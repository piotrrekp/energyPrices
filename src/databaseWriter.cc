#include "databaseWriter.h"
#include "stringUtils.h"

#include <sqlite3.h>

#include <chrono>
#include <cmath>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>

namespace {

using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

void check(sqlite3* db, int result) {
    if (result != SQLITE_OK) {
        throw std::runtime_error{std::string{"SQLite write failed: "} + sqlite3_errmsg(db)};
    }
}

Statement prepare(sqlite3* db, const char* sql) {
    sqlite3_stmt* raw = nullptr;
    const auto result = sqlite3_prepare_v2(db, sql, -1, &raw, nullptr);
    Statement statement{raw, &sqlite3_finalize};
    check(db, result);
    return statement;
}

void bindText(sqlite3* db, sqlite3_stmt* statement, int index, const std::string& value) {
    check(db, sqlite3_bind_text64(statement, index, value.data(), value.size(),
        SQLITE_TRANSIENT, SQLITE_UTF8));
}

void execute(sqlite3* db, sqlite3_stmt* statement) {
    if (sqlite3_step(statement) != SQLITE_DONE) {
        throw std::runtime_error{std::string{"SQLite write failed: "} + sqlite3_errmsg(db)};
    }
}

} // namespace

databaseWriter::databaseWriter(const std::filesystem::path& path)
    : connection{path, databaseAccess::writeOnly} {
    check(connection.get(), sqlite3_exec(connection.get(),
        "PRAGMA foreign_keys = ON", nullptr, nullptr, nullptr));
}

void databaseWriter::store(const fetchedDay& data) {
    if (!data.date.ok()) {
        throw std::invalid_argument{"Invalid delivery date"};
    }
    // Until empty snapshots have an explicit domain meaning, preserve stored data.
    if (data.prices.empty()) {
        throw std::invalid_argument{"Cannot store an empty price snapshot"};
    }
    std::set<std::string> periods;
    for (const auto& price : data.prices) {
        if (price.period.empty() || !std::isfinite(price.pricePerMWh)) {
            throw std::invalid_argument{"Invalid price or empty time period"};
        }
        if (!periods.insert(price.period).second) {
            throw std::invalid_argument{"Duplicate time period"};
        }
    }

    auto* db = connection.get();
    const auto date = stringUtils::getDate(data.date);
    const auto timestamp = std::chrono::floor<std::chrono::seconds>(
        data.timestamp.time_since_epoch()).count();

    check(db, sqlite3_exec(db, "BEGIN IMMEDIATE", nullptr, nullptr, nullptr));
    try {
        auto day = prepare(db,
            "INSERT INTO price_days(delivery_date, complete, updated_at) "
            "VALUES (?1, ?2, datetime(?3, 'unixepoch')) "
            "ON CONFLICT(delivery_date) DO UPDATE SET "
            "complete = excluded.complete, updated_at = excluded.updated_at");
        bindText(db, day.get(), 1, date);
        check(db, sqlite3_bind_int(day.get(), 2, data.completed ? 1 : 0));
        check(db, sqlite3_bind_int64(day.get(), 3, timestamp));
        execute(db, day.get());

        auto insert = prepare(db,
            "INSERT INTO hourly_prices(delivery_date, time_period, price) VALUES (?1, ?2, ?3) "
            "ON CONFLICT(delivery_date, time_period) DO UPDATE SET price = excluded.price");
        bindText(db, insert.get(), 1, date);
        for (const auto& price : data.prices) {
            bindText(db, insert.get(), 2, price.period);
            check(db, sqlite3_bind_double(insert.get(), 3, price.pricePerMWh));
            execute(db, insert.get());
            check(db, sqlite3_reset(insert.get()));
        }
        check(db, sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr));
    } catch (...) {
        // Preserve the original error; rollback must not throw during unwinding.
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        throw;
    }
}

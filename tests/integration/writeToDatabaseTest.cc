#include <gtest/gtest.h>
#include <sqlite3.h>
#include <unistd.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "databaseWriter.h"

namespace {

class databaseWriterTest : public testing::Test {
protected:
    std::filesystem::path path;
    std::unique_ptr<sqliteConnection> db;
    std::unique_ptr<databaseWriter> writer;

    void SetUp() override {
        auto pattern = (std::filesystem::temp_directory_path() / "energy-prices-test-XXXXXX").string();
        const int fd = mkstemp(pattern.data());
        ASSERT_NE(fd, -1);
        path = pattern;
        close(fd);
        db = std::make_unique<sqliteConnection>(path, databaseAccess::writeOnly);
        std::ifstream input{DATABASE_SCHEMA_PATH};
        ASSERT_TRUE(input.is_open());
        const std::string schema{std::istreambuf_iterator<char>{input},
            std::istreambuf_iterator<char>{}};
        exec(schema);
        writer = std::make_unique<databaseWriter>(path);
    }

    void TearDown() override {
        writer.reset();
        db.reset();
        if (!path.empty()) {
            std::error_code error;
            std::filesystem::remove(path, error);
            std::filesystem::remove(path.string() + "-wal", error);
            std::filesystem::remove(path.string() + "-shm", error);
        }
    }

    void exec(const std::string& sql) {
        if (sqlite3_exec(db->get(), sql.c_str(), nullptr, nullptr, nullptr) != SQLITE_OK) {
            throw std::runtime_error{sqlite3_errmsg(db->get())};
        }
    }

    std::vector<std::vector<std::string>> query(const char* sql) {
        sqlite3_stmt* raw = nullptr;
        const int result = sqlite3_prepare_v2(db->get(), sql, -1, &raw, nullptr);
        std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> statement{raw, &sqlite3_finalize};
        if (result != SQLITE_OK) {
            throw std::runtime_error{sqlite3_errmsg(db->get())};
        }
        std::vector<std::vector<std::string>> rows;
        int step;
        while ((step = sqlite3_step(statement.get())) == SQLITE_ROW) {
            std::vector<std::string> row;
            for (int column = 0; column < sqlite3_column_count(statement.get()); ++column) {
                const auto* text = sqlite3_column_text(statement.get(), column);
                row.emplace_back(text ? reinterpret_cast<const char*>(text) : "NULL");
            }
            rows.push_back(std::move(row));
        }
        if (step != SQLITE_DONE) {
            throw std::runtime_error{sqlite3_errmsg(db->get())};
        }
        return rows;
    }

    fetchedDay snapshot() const {
        return {std::chrono::year{2026} / 9 / 30,
            std::chrono::sys_days{std::chrono::year{2026} / 9 / 29}
                + std::chrono::hours{12} + std::chrono::minutes{34} + std::chrono::seconds{56},
            {{"2026-09-30_H1", 345.67}, {"2026-09-30_H2", 200.0}}, false};
    }
};

TEST_F(databaseWriterTest, StoresCompleteDayAndFetchTimestampPersistently) {
    auto data = snapshot();
    data.prices.clear();
    for (int hour = 1; hour <= 24; ++hour) {
        data.prices.push_back({"2026-09-30_H" + std::to_string(hour), 345.67});
    }
    data.completed = true;
    writer->store(data);
    writer.reset();
    db = std::make_unique<sqliteConnection>(path, databaseAccess::readOnly);

    const auto days = query("SELECT delivery_date, complete, updated_at FROM price_days");
    ASSERT_EQ(days.size(), 1u);
    EXPECT_EQ(days[0], (std::vector<std::string>{"2026-09-30", "1", "2026-09-29 12:34:56"}));
    const auto prices = query("SELECT price FROM hourly_prices");
    ASSERT_EQ(prices.size(), 24u);
    for (const auto& row : prices) {
        EXPECT_DOUBLE_EQ(std::stod(row[0]), 345.67);
    }
}

TEST_F(databaseWriterTest, StoresPartialSnapshotIncludingZeroAndNegativePrices) {
    auto data = snapshot();
    data.prices = {{"2026-09-30_H1", 0.0}, {"2026-09-30_H3", -50.5}};
    writer->store(data);
    const auto rows = query("SELECT time_period, price FROM hourly_prices ORDER BY time_period");
    ASSERT_EQ(rows.size(), 2u);
    EXPECT_EQ(rows[0][0], "2026-09-30_H1");
    EXPECT_DOUBLE_EQ(std::stod(rows[0][1]), 0.0);
    EXPECT_EQ(rows[1][0], "2026-09-30_H3");
    EXPECT_DOUBLE_EQ(std::stod(rows[1][1]), -50.5);
    EXPECT_EQ(query("SELECT complete FROM price_days")[0][0], "0");
}

TEST_F(databaseWriterTest, MergesPartialUpdateWithoutLosingPeriodsOrChangingOtherDays) {
    auto data = snapshot();
    data.completed = true;
    writer->store(data);
    auto other = snapshot();
    other.date = std::chrono::year{2026} / 10 / 1;
    other.prices = {{"2026-10-01_H1", 123.0}};
    writer->store(other);
    data.prices = {{"2026-09-30_H2", 456.78}, {"2026-09-30_H3", 0.0}};
    data.completed = false;
    data.timestamp += std::chrono::hours{1};
    writer->store(data);

    const auto rows = query("SELECT delivery_date, time_period, price FROM hourly_prices ORDER BY delivery_date, time_period");
    ASSERT_EQ(rows.size(), 4u);
    EXPECT_EQ(rows[0][1], "2026-09-30_H1");
    EXPECT_DOUBLE_EQ(std::stod(rows[0][2]), 345.67);
    EXPECT_EQ(rows[1][1], "2026-09-30_H2");
    EXPECT_DOUBLE_EQ(std::stod(rows[1][2]), 456.78);
    EXPECT_EQ(rows[2][1], "2026-09-30_H3");
    EXPECT_DOUBLE_EQ(std::stod(rows[2][2]), 0.0);
    EXPECT_EQ(rows[3][0], "2026-10-01");
    EXPECT_DOUBLE_EQ(std::stod(rows[3][2]), 123.0);
    EXPECT_EQ(query("SELECT complete, updated_at FROM price_days WHERE delivery_date='2026-09-30'")[0],
        (std::vector<std::string>{"0", "2026-09-29 13:34:56"}));
}

TEST_F(databaseWriterTest, RepeatedSnapshotDoesNotCreateDuplicates) {
    writer->store(snapshot());
    writer->store(snapshot());
    EXPECT_EQ(query("SELECT * FROM price_days").size(), 1u);
    EXPECT_EQ(query("SELECT * FROM hourly_prices").size(), 2u);
}

TEST_F(databaseWriterTest, DuplicatePeriodIsRejectedWithoutChangesAndAllowsRetry) {
    writer->store(snapshot());
    const auto beforeDays = query("SELECT * FROM price_days");
    const auto beforePrices = query("SELECT * FROM hourly_prices ORDER BY time_period");
    auto data = snapshot();
    data.completed = true;
    data.timestamp += std::chrono::hours{1};
    data.prices = {{"2026-09-30_H1", 999.0}, {"2026-09-30_H1", 888.0}};
    EXPECT_THROW(writer->store(data), std::invalid_argument);
    EXPECT_EQ(query("SELECT * FROM price_days"), beforeDays);
    EXPECT_EQ(query("SELECT * FROM hourly_prices ORDER BY time_period"), beforePrices);
    data.prices.pop_back();
    EXPECT_NO_THROW(writer->store(data));
    EXPECT_EQ(query("SELECT complete FROM price_days")[0][0], "1");
}

TEST_F(databaseWriterTest, FailedFirstSnapshotLeavesNoDayOrPrices) {
    auto data = snapshot();
    data.prices.push_back(data.prices.front());
    EXPECT_THROW(writer->store(data), std::invalid_argument);
    EXPECT_TRUE(query("SELECT * FROM price_days").empty());
    EXPECT_TRUE(query("SELECT * FROM hourly_prices").empty());
}

TEST_F(databaseWriterTest, DatabaseFailureRollsBackWholeSnapshot) {
    writer->store(snapshot());
    const auto before = query("SELECT * FROM hourly_prices ORDER BY time_period");
    const auto beforeDays = query("SELECT * FROM price_days");
    exec("CREATE TRIGGER reject_price BEFORE INSERT ON hourly_prices "
         "WHEN NEW.price = 999 BEGIN SELECT RAISE(ABORT, 'test write failure'); END");
    auto data = snapshot();
    data.completed = true;
    data.timestamp += std::chrono::hours{1};
    data.prices[0].pricePerMWh = 111;
    data.prices[1].pricePerMWh = 999;
    EXPECT_THROW(writer->store(data), std::runtime_error);
    EXPECT_EQ(query("SELECT * FROM hourly_prices ORDER BY time_period"), before);
    EXPECT_EQ(query("SELECT * FROM price_days"), beforeDays);
}

TEST_F(databaseWriterTest, EmptySnapshotIsRejectedWithoutDeletingExistingPrices) {
    writer->store(snapshot());
    auto data = snapshot();
    data.prices.clear();
    EXPECT_THROW(writer->store(data), std::invalid_argument);
    EXPECT_EQ(query("SELECT * FROM hourly_prices").size(), 2u);
}

TEST_F(databaseWriterTest, InvalidDateAndInvalidPricesAreRejected) {
    auto data = snapshot();
    data.date = std::chrono::year{2026} / 2 / 30;
    EXPECT_THROW(writer->store(data), std::invalid_argument);
    data = snapshot();
    data.prices[0].period.clear();
    EXPECT_THROW(writer->store(data), std::invalid_argument);
    for (double value : {std::numeric_limits<double>::infinity(),
            -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()}) {
        data = snapshot();
        data.prices[0].pricePerMWh = value;
        EXPECT_THROW(writer->store(data), std::invalid_argument);
    }
    EXPECT_TRUE(query("SELECT * FROM price_days").empty());
}

TEST_F(databaseWriterTest, BindsPeriodAsDataInsteadOfSql) {
    auto data = snapshot();
    data.prices = {{"period'); DROP TABLE price_days; --", 123.0}};
    writer->store(data);
    EXPECT_EQ(query("SELECT time_period FROM hourly_prices")[0][0], data.prices[0].period);
    EXPECT_EQ(query("SELECT * FROM price_days").size(), 1u);
}

TEST_F(databaseWriterTest, ReportsMissingSchema) {
    exec("DROP TABLE hourly_prices; DROP TABLE price_days");
    EXPECT_THROW(writer->store(snapshot()), std::runtime_error);
}

} // namespace

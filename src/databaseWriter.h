#ifndef _DATABASEWRITER_H_
#define _DATABASEWRITER_H_

#include "sqliteConnection.h"
#include "daySaver.h"

#include <filesystem>

class databaseWriter : public daySaver {
public:
    databaseWriter(const std::filesystem::path& path);
    // Merges a nonempty update atomically, preserving absent periods (PLN/MWh).
    // complete describes the latest response, not accumulated period coverage.
    // updated_at stores data.timestamp in UTC with second precision.
    // Invalid input throws invalid_argument; SQLite failures throw runtime_error.
    void store(const fetchedDay &data) override;

private:
    sqliteConnection connection;
};

#endif // _DATABASEWRITER_H_

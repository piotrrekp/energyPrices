#ifndef _SQLITECONNECTION_H_
#define _SQLITECONNECTION_H_

#include <filesystem>

struct sqlite3;

enum class databaseAccess {
    readOnly,
    writeOnly
};

class sqliteConnection {
public:
    sqliteConnection(const std::filesystem::path& databasePath, const databaseAccess access);
    sqliteConnection(const sqliteConnection&) = delete;
    sqliteConnection operator=(const sqliteConnection&) = delete;
    sqliteConnection(const sqliteConnection&&) = delete;
    sqliteConnection operator=(const sqliteConnection&&) = delete;

    [[nodiscard]] sqlite3* get() const;

private:
    struct sqliteDeleter {
	void operator()(sqlite3 *connection) const;
    };

    std::unique_ptr<sqlite3, sqliteDeleter> connection;
};

#endif // _SQLITECONNECTION_H_

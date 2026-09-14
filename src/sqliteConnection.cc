#include "sqliteConnection.h"

#include <sqlite3.h>

void sqliteConnection::sqliteDeleter::operator()(sqlite3 *connection) const {
	sqlite3_close_v2(connection);
}

sqliteConnection::sqliteConnection(
		const std::filesystem::path& databasePath,
		const databaseAccess access) {
	sqlite3 *conn = nullptr;

	const int flag = access == databaseAccess::readOnly
		? SQLITE_OPEN_READONLY
		: SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
	const int result = sqlite3_open_v2(
			databasePath.string().c_str(),
			&conn,
			flag,
			nullptr);

	connection.reset(conn);
	if (result != SQLITE_OK) {
		const std::string error = conn == nullptr
			? sqlite3_errmsg(conn)
			: sqlite3_errstr(result);

		throw std::runtime_error{
			"Failed to open SQLite database '" +
			databasePath.string() + "' :" +
			error
		};
	}
	sqlite3_busy_timeout(connection.get(), 5000);
}

sqlite3* sqliteConnection::get() const {
	return connection.get();

}

#ifndef _DATABASEWRITER_H_
#define _DATABASEWRITER_H_

#include "sqliteConnection.h"

#include <filesystem>

class databaseWriter {
public:
    databaseWriter(const std::filesystem::path& path);

private:
    sqliteConnection connection;
};

#endif // _DATABASEWRITER_H_

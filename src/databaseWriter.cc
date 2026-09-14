#include "databaseWriter.h"



databaseWriter::databaseWriter(const std::filesystem::path& path)
	: connection{path, databaseAccess::writeOnly} {}

#include "database.h"
#include <stdexcept>
#include <algorithm>

void Database::load(const std::string& filepath) {
    Table table = CSVParser::parse(filepath);
    std::string name = extractTableName(filepath);
    tables[name] = table;
}

const Table& Database::getTable(const std::string& name) const {
    auto it = tables.find(name);
    if (it == tables.end())
        throw std::runtime_error("Table not found: " + name);
    return it->second;
}

bool Database::hasTable(const std::string& name) const {
    return tables.find(name) != tables.end();
}

std::string Database::extractTableName(const std::string& filepath) {
    // Find last '/' or '\\'
    size_t slashPos = filepath.find_last_of("/\\");
    std::string filename = (slashPos == std::string::npos)
        ? filepath
        : filepath.substr(slashPos + 1);

    // Remove extension
    size_t dotPos = filename.find_last_of('.');
    return (dotPos == std::string::npos) ? filename : filename.substr(0, dotPos);
}
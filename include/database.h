#pragma once
#include <string>
#include <unordered_map>
#include "csv_parser.h"

class Database {
public:
    // Load a CSV file and store it under the table name (filename without extension)
    void load(const std::string& filepath);

    // Get a table by name, throws std::runtime_error if not found
    const Table& getTable(const std::string& name) const;

    // Check if a table exists
    bool hasTable(const std::string& name) const;

private:
    std::unordered_map<std::string, Table> tables;

    // Extract table name from filepath e.g. "data/employees.csv" -> "employees"
    static std::string extractTableName(const std::string& filepath);
};
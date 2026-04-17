#include "csv_parser.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

Table CSVParser::parse(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filepath);

    Table table;
    std::string line;

    // First line = headers
    if (!std::getline(file, line))
        throw std::runtime_error("Empty file: " + filepath);
    table.headers = splitLine(line);

    // Remaining lines = rows
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        auto tokens = splitLine(line);
        std::vector<Value> row;
        for (auto& token : tokens)
            row.push_back(inferType(token));
        table.rows.push_back(row);
    }
    return table;
}

std::vector<std::string> CSVParser::splitLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field); // last field
    return fields;
}

Value CSVParser::inferType(const std::string& token) {
    // Boolean
    if (token == "true" || token == "True") return true;
    if (token == "false" || token == "False") return false;

    // Integer
    try {
        size_t pos;
        int i = std::stoi(token, &pos);
        if (pos == token.size()) return i;
    } catch (...) {}

    // Double
    try {
        size_t pos;
        double d = std::stod(token, &pos);
        if (pos == token.size()) return d;
    } catch (...) {}

    // Default: string
    return token;
}
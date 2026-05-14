#pragma once
#include <string>
#include <vector>
#include <variant>

// A cell can be int, double, bool, or string
using Value = std::variant<int, double, bool, std::string>;

struct Table {
    std::vector<std::string> headers;
    std::vector<std::vector<Value>> rows;
};

class CSVParser {
public:
    // Parse a CSV file and return a Table
    // Throws std::runtime_error on failure
    static Table parse(const std::string& filepath);

private:
    static std::vector<std::string> splitLine(const std::string& line);
    static Value inferType(const std::string& token);
};
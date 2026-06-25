#include "database.h"
#include "query_parser.h"
#include "executor.h"
#include "value_util.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace {

void printHelp() {
    std::cout <<
        "Commands:\n"
        "  LOAD <path.csv>      load a CSV file as a table (named after the file)\n"
        "  .tables              (informational) describe how tables are named\n"
        "  .help                show this help\n"
        "  .exit / .quit        leave the shell\n\n"
        "Queries:\n"
        "  SELECT <cols|*|AGG(col)> FROM <table>\n"
        "         [WHERE <col> <op> <value> [AND|OR ...]]\n"
        "         [ORDER BY <col> [ASC|DESC]] [LIMIT <n>]\n"
        "  Operators: =  !=  <  >  <=  >=  LIKE   Aggregates: COUNT SUM AVG MIN MAX\n"
        "  Example: SELECT name, salary FROM employees WHERE salary > 60000 ORDER BY salary DESC\n";
}

// Print a ResultSet as an aligned text table.
void printResult(const ResultSet& rs) {
    if (rs.headers.empty()) { std::cout << "(no columns)\n"; return; }

    std::vector<size_t> width(rs.headers.size());
    for (size_t c = 0; c < rs.headers.size(); ++c)
        width[c] = rs.headers[c].size();
    std::vector<std::vector<std::string>> cells;
    for (const auto& row : rs.rows) {
        std::vector<std::string> line;
        for (size_t c = 0; c < row.size(); ++c) {
            std::string s = valueutil::toString(row[c]);
            width[c] = std::max(width[c], s.size());
            line.push_back(s);
        }
        cells.push_back(std::move(line));
    }

    auto pad = [](const std::string& s, size_t w) {
        return s + std::string(w - s.size(), ' ');
    };

    std::cout << "  ";
    for (size_t c = 0; c < rs.headers.size(); ++c)
        std::cout << pad(rs.headers[c], width[c]) << (c + 1 < rs.headers.size() ? "  " : "");
    std::cout << "\n  ";
    for (size_t c = 0; c < rs.headers.size(); ++c)
        std::cout << std::string(width[c], '-') << (c + 1 < rs.headers.size() ? "  " : "");
    std::cout << "\n";

    for (const auto& line : cells) {
        std::cout << "  ";
        for (size_t c = 0; c < line.size(); ++c)
            std::cout << pad(line[c], width[c]) << (c + 1 < line.size() ? "  " : "");
        std::cout << "\n";
    }
    std::cout << "  (" << rs.rows.size() << (rs.rows.size() == 1 ? " row)\n" : " rows)\n");
}

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

bool startsWithKeyword(const std::string& s, const std::string& kw) {
    if (s.size() < kw.size()) return false;
    for (size_t i = 0; i < kw.size(); ++i)
        if (std::toupper(static_cast<unsigned char>(s[i])) != kw[i]) return false;
    return true;
}

// Handle one line of input. Returns false if the shell should exit.
bool handleLine(Database& db, const std::string& raw) {
    std::string line = trim(raw);
    if (line.empty()) return true;

    if (line == ".exit" || line == ".quit") return false;
    if (line == ".help") { printHelp(); return true; }
    if (line == ".tables") {
        std::cout << "Tables are named after the loaded file: data/employees.csv -> employees\n";
        return true;
    }

    if (startsWithKeyword(line, "LOAD")) {
        std::string path = trim(line.substr(4));
        if (!path.empty() && (path.front() == '"' || path.front() == '\'') && path.size() >= 2)
            path = path.substr(1, path.size() - 2);
        try {
            db.load(path);
            std::cout << "Loaded '" << path << "'.\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
        return true;
    }

    try {
        QueryPlan plan = QueryParser::parse(line);
        ResultSet rs = Executor::execute(plan, db);
        printResult(rs);
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << "\n";
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    Database db;

    // Any CSV paths given on the command line are auto-loaded.
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        try {
            db.load(arg);
            std::cout << "Loaded '" << arg << "'.\n";
        } catch (const std::exception& e) {
            std::cout << "Error loading '" << arg << "': " << e.what() << "\n";
        }
    }

    std::cout << "csvdb - interactive query shell. Type .help for help, .exit to quit.\n";

    std::string line;
    while (true) {
        std::cout << "db> " << std::flush;
        if (!std::getline(std::cin, line)) break;   // EOF (e.g. piped input)
        if (!handleLine(db, line)) break;
    }
    return 0;
}

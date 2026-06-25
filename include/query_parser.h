#pragma once
#include "query.h"
#include <string>
#include <vector>

// Parses a SQL-like query string into a QueryPlan.
// Grammar:
//   SELECT <list> FROM <table>
//   [WHERE <cond> {AND|OR <cond>}]
//   [ORDER BY <col> [ASC|DESC]]
//   [LIMIT <n>]
// Throws std::runtime_error with a descriptive message on malformed input.
class QueryParser {
public:
    static QueryPlan parse(const std::string& query);

private:
    static std::vector<std::string> tokenize(const std::string& query);
};

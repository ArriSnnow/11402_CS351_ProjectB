#pragma once
#include "query.h"
#include "database.h"
#include <string>
#include <vector>

// The result of running a query: a small in-memory table.
struct ResultSet {
    std::vector<std::string> headers;
    std::vector<std::vector<Value>> rows;
};

// Runs a QueryPlan against a Database and produces a ResultSet.
// Throws std::runtime_error on semantic errors (unknown table/column,
// aggregate over a non-numeric column, etc.).
class Executor {
public:
    static ResultSet execute(const QueryPlan& plan, const Database& db);

private:
    // Case-insensitive column lookup; throws if the column is absent.
    static size_t columnIndex(const Table& table, const std::string& name);
    static bool rowMatches(const QueryPlan& plan, const Table& table,
                           const std::vector<Value>& row);
};

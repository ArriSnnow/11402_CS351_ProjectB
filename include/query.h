#pragma once
#include "csv_parser.h"
#include <string>
#include <vector>

// Structured representation of a parsed SQL-like query.

enum class Comparator { EQ, NE, LT, GT, LE, GE, LIKE };
enum class Logic { AND, OR };
enum class AggFunc { NONE, COUNT, SUM, AVG, MIN, MAX };

// One item in the SELECT list: either a plain column or an aggregate call.
struct SelectItem {
    AggFunc func = AggFunc::NONE;   // NONE => plain column
    std::string column;             // column name, or "*" for COUNT(*)
};

// A single WHERE comparison: column <op> literal.
struct Condition {
    std::string column;
    Comparator op;
    Value literal;
};

struct OrderBy {
    bool present = false;
    std::string column;
    bool descending = false;
};

struct QueryPlan {
    bool selectAll = false;                 // SELECT *
    std::vector<SelectItem> items;          // populated when selectAll == false
    bool hasAggregate = false;              // any item is an aggregate
    std::string table;

    std::vector<Condition> conditions;      // WHERE conditions
    std::vector<Logic> connectors;          // size == conditions.size() - 1

    OrderBy orderBy;
    int limit = -1;                         // -1 => no limit
};

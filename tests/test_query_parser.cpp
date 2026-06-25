#include <cassert>
#include <iostream>
#include "query_parser.h"

void test_select_all() {
    QueryPlan p = QueryParser::parse("SELECT * FROM employees");
    assert(p.selectAll);
    assert(p.table == "employees");
    assert(!p.hasAggregate);
    std::cout << "PASS: select all\n";
}

void test_select_columns() {
    QueryPlan p = QueryParser::parse("SELECT name, salary FROM employees");
    assert(!p.selectAll);
    assert(p.items.size() == 2);
    assert(p.items[0].column == "name");
    assert(p.items[1].column == "salary");
    std::cout << "PASS: select columns\n";
}

void test_where_and_order_limit() {
    QueryPlan p = QueryParser::parse(
        "SELECT name FROM employees WHERE salary > 60000 ORDER BY salary DESC LIMIT 2");
    assert(p.conditions.size() == 1);
    assert(p.conditions[0].column == "salary");
    assert(p.conditions[0].op == Comparator::GT);
    assert(p.orderBy.present);
    assert(p.orderBy.descending);
    assert(p.limit == 2);
    std::cout << "PASS: where + order by + limit\n";
}

void test_logical_connectors() {
    QueryPlan p = QueryParser::parse(
        "SELECT * FROM t WHERE a = 1 AND b = 2 OR c = 3");
    assert(p.conditions.size() == 3);
    assert(p.connectors.size() == 2);
    assert(p.connectors[0] == Logic::AND);
    assert(p.connectors[1] == Logic::OR);
    std::cout << "PASS: logical connectors\n";
}

void test_aggregate() {
    QueryPlan p = QueryParser::parse("SELECT AVG(salary) FROM employees");
    assert(p.hasAggregate);
    assert(p.items.size() == 1);
    assert(p.items[0].func == AggFunc::AVG);
    assert(p.items[0].column == "salary");
    std::cout << "PASS: aggregate parse\n";
}

void test_quoted_string_literal() {
    QueryPlan p = QueryParser::parse("SELECT * FROM t WHERE name = 'Smith, John'");
    assert(p.conditions.size() == 1);
    assert(std::get<std::string>(p.conditions[0].literal) == "Smith, John");
    std::cout << "PASS: quoted string literal\n";
}

void test_like_operator() {
    QueryPlan p = QueryParser::parse("SELECT * FROM t WHERE name LIKE 'A%'");
    assert(p.conditions[0].op == Comparator::LIKE);
    std::cout << "PASS: like operator\n";
}

void test_invalid_missing_from() {
    try {
        QueryParser::parse("SELECT name employees");
        assert(false);
    } catch (const std::runtime_error&) {
        std::cout << "PASS: invalid (missing FROM) throws\n";
    }
}

void test_invalid_mixed_select() {
    try {
        QueryParser::parse("SELECT name, COUNT(*) FROM t");
        assert(false);
    } catch (const std::runtime_error&) {
        std::cout << "PASS: invalid (mixed aggregate/plain) throws\n";
    }
}

int main() {
    test_select_all();
    test_select_columns();
    test_where_and_order_limit();
    test_logical_connectors();
    test_aggregate();
    test_quoted_string_literal();
    test_like_operator();
    test_invalid_missing_from();
    test_invalid_mixed_select();
    std::cout << "\nAll tests passed!\n";
    return 0;
}

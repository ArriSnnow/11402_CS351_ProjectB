#include <cassert>
#include <iostream>
#include <cmath>
#include "query_parser.h"
#include "executor.h"
#include "value_util.h"

#ifndef PROJECT_DATA_DIR
#define PROJECT_DATA_DIR "data"
#endif
static const std::string CSV = std::string(PROJECT_DATA_DIR) + "/employees.csv";

static ResultSet run(Database& db, const std::string& q) {
    return Executor::execute(QueryParser::parse(q), db);
}

void test_select_all() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT * FROM employees");
    assert(r.headers.size() == 5);
    assert(r.rows.size() == 5);
    std::cout << "PASS: select all\n";
}

void test_projection() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name, salary FROM employees");
    assert(r.headers.size() == 2);
    assert(r.headers[0] == "name" && r.headers[1] == "salary");
    std::cout << "PASS: projection\n";
}

void test_where_numeric() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name FROM employees WHERE salary > 60000");
    assert(r.rows.size() == 4); // all except Dave (55000)
    std::cout << "PASS: where numeric\n";
}

void test_where_string_eq() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name FROM employees WHERE department = 'Engineering'");
    assert(r.rows.size() == 2); // Alice, Carol
    std::cout << "PASS: where string equality\n";
}

void test_where_and() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name FROM employees WHERE salary > 60000 AND department = 'Marketing'");
    assert(r.rows.size() == 2); // Bob, Smith John
    std::cout << "PASS: where AND\n";
}

void test_where_or() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name FROM employees WHERE department = 'HR' OR department = 'Engineering'");
    assert(r.rows.size() == 3); // Dave, Alice, Carol
    std::cout << "PASS: where OR\n";
}

void test_like() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name FROM employees WHERE name LIKE 'A%'");
    assert(r.rows.size() == 1); // Alice
    std::cout << "PASS: like prefix\n";
}

void test_order_limit() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT name, salary FROM employees ORDER BY salary DESC LIMIT 2");
    assert(r.rows.size() == 2);
    assert(std::get<std::string>(r.rows[0][0]) == "Carol"); // highest
    assert(std::get<std::string>(r.rows[1][0]) == "Alice"); // second
    std::cout << "PASS: order by desc + limit\n";
}

void test_count() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT COUNT(*) FROM employees");
    assert(r.rows.size() == 1);
    assert(std::get<int>(r.rows[0][0]) == 5);
    std::cout << "PASS: count\n";
}

void test_count_with_where() {
    Database db; db.load(CSV);
    ResultSet r = run(db, "SELECT COUNT(*) FROM employees WHERE active = true");
    assert(std::get<int>(r.rows[0][0]) == 3); // Alice, Carol, Dave
    std::cout << "PASS: count with where\n";
}

void test_aggregates_numeric() {
    Database db; db.load(CSV);
    double sum = std::get<double>(run(db, "SELECT SUM(salary) FROM employees").rows[0][0]);
    double avg = std::get<double>(run(db, "SELECT AVG(salary) FROM employees").rows[0][0]);
    double mn  = std::get<double>(run(db, "SELECT MIN(salary) FROM employees").rows[0][0]);
    double mx  = std::get<double>(run(db, "SELECT MAX(salary) FROM employees").rows[0][0]);
    assert(std::fabs(sum - 360001.25) < 0.01);
    assert(std::fabs(avg - 72000.25) < 0.01);
    assert(std::fabs(mn - 55000.00) < 0.01);
    assert(std::fabs(mx - 91000.75) < 0.01);
    std::cout << "PASS: sum/avg/min/max\n";
}

void test_unknown_column() {
    Database db; db.load(CSV);
    try {
        run(db, "SELECT nope FROM employees");
        assert(false);
    } catch (const std::runtime_error&) {
        std::cout << "PASS: unknown column throws\n";
    }
}

void test_unknown_table() {
    Database db; db.load(CSV);
    try {
        run(db, "SELECT * FROM ghosts");
        assert(false);
    } catch (const std::runtime_error&) {
        std::cout << "PASS: unknown table throws\n";
    }
}

int main() {
    test_select_all();
    test_projection();
    test_where_numeric();
    test_where_string_eq();
    test_where_and();
    test_where_or();
    test_like();
    test_order_limit();
    test_count();
    test_count_with_where();
    test_aggregates_numeric();
    test_unknown_column();
    test_unknown_table();
    std::cout << "\nAll tests passed!\n";
    return 0;
}

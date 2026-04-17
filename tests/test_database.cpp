#include <cassert>
#include <iostream>
#include "database.h"

const std::string CSV_PATH = "/Users/Arri/git repo/11402_CS351_ProjectB/data/employees.csv";

void test_load_table() {
    Database db;
    db.load(CSV_PATH);
    assert(db.hasTable("employees"));
    std::cout << "PASS: load table\n";
}

void test_get_table() {
    Database db;
    db.load(CSV_PATH);
    const Table& t = db.getTable("employees");
    assert(t.rows.size() == 5);
    assert(t.headers.size() == 5);
    std::cout << "PASS: get table\n";
}

void test_table_not_found() {
    Database db;
    try {
        db.getTable("nonexistent");
        assert(false); // should not reach here
    } catch (const std::runtime_error& e) {
        std::cout << "PASS: table not found error\n";
    }
}

void test_overwrite_table() {
    Database db;
    db.load(CSV_PATH);
    db.load(CSV_PATH); // load again
    const Table& t = db.getTable("employees");
    assert(t.rows.size() == 5); // still 5 rows, not 10
    std::cout << "PASS: overwrite table\n";
}

int main() {
    test_load_table();
    test_get_table();
    test_table_not_found();
    test_overwrite_table();
    std::cout << "\nAll tests passed!\n";
    return 0;
}
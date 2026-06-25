#include <cassert>
#include <iostream>
#include "csv_parser.h"

#ifndef PROJECT_DATA_DIR
#define PROJECT_DATA_DIR "data"
#endif
static const std::string CSV = std::string(PROJECT_DATA_DIR) + "/employees.csv";

void test_header_detection() {
    Table t = CSVParser::parse(CSV);
    assert(t.headers[0] == "id");
    assert(t.headers[1] == "name");
    assert(t.headers[2] == "department");
    assert(t.headers.size() == 5);
    std::cout << "PASS: header detection\n";
}

void test_row_count() {
    Table t = CSVParser::parse(CSV);
    assert(t.rows.size() == 5);
    std::cout << "PASS: row count\n";
}

void test_type_inference() {
    Table t = CSVParser::parse(CSV);
    // id=1 should be int
    assert(std::holds_alternative<int>(t.rows[0][0]));
    // salary should be double
    assert(std::holds_alternative<double>(t.rows[0][3]));
    // active should be bool
    assert(std::holds_alternative<bool>(t.rows[0][4]));
    // name should be string
    assert(std::holds_alternative<std::string>(t.rows[0][1]));
    std::cout << "PASS: type inference\n";
}

void test_quoted_field() {
    Table t = CSVParser::parse(CSV);
    // Row 4 (index 4): "Smith, John" should be parsed as one field
    assert(std::get<std::string>(t.rows[4][1]) == "Smith, John");
    std::cout << "PASS: quoted field with comma\n";
}

int main() {
    test_header_detection();
    test_row_count();
    test_type_inference();
    test_quoted_field();
    std::cout << "\nAll tests passed!\n";
    return 0;
}
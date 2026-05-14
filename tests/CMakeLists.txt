# Tests will be added in later phase (Execution Engine)
# add_executable(test_csv_parser test_csv_parser.cpp)
# target_include_directories(test_csv_parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
# add_test(NAME test_csv_parser COMMAND test_csv_parser)

add_executable(test_csv_parser 
    test_csv_parser.cpp
    ../src/csv_parser.cpp
)
target_include_directories(test_csv_parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME test_csv_parser COMMAND test_csv_parser)

add_executable(test_database
    test_database.cpp
    ../src/database.cpp
    ../src/csv_parser.cpp
)
target_include_directories(test_database PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_test(NAME test_database COMMAND test_database)
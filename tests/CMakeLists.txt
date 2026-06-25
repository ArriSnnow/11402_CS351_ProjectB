# Each test is a standalone executable using <cassert>; no external framework.

set(ENGINE_SRC
    ../src/csv_parser.cpp
    ../src/database.cpp
    ../src/query_parser.cpp
    ../src/executor.cpp
)
set(INC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(test_csv_parser test_csv_parser.cpp ../src/csv_parser.cpp)
target_include_directories(test_csv_parser PRIVATE ${INC})
add_test(NAME test_csv_parser COMMAND test_csv_parser)

add_executable(test_database test_database.cpp ../src/database.cpp ../src/csv_parser.cpp)
target_include_directories(test_database PRIVATE ${INC})
add_test(NAME test_database COMMAND test_database)

add_executable(test_query_parser test_query_parser.cpp ../src/query_parser.cpp)
target_include_directories(test_query_parser PRIVATE ${INC})
add_test(NAME test_query_parser COMMAND test_query_parser)

add_executable(test_executor test_executor.cpp ${ENGINE_SRC})
target_include_directories(test_executor PRIVATE ${INC})
add_test(NAME test_executor COMMAND test_executor)

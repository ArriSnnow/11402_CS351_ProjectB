cmake_minimum_required(VERSION 3.10)
project(CS351_ProjectB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Engine sources shared by the executable and the tests.
set(ENGINE_SOURCES
    src/csv_parser.cpp
    src/database.cpp
    src/query_parser.cpp
    src/executor.cpp
)

# Main CLI executable
add_executable(csv_db src/main.cpp ${ENGINE_SOURCES})
target_include_directories(csv_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Absolute path to the sample data, so tests run from any working directory.
add_compile_definitions(PROJECT_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data")

# Enable testing
enable_testing()
add_subdirectory(tests)

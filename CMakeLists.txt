cmake_minimum_required(VERSION 3.10)
project(CS351_ProjectB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(csv_db src/main.cpp src/csv_parser.cpp)
target_include_directories(csv_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)
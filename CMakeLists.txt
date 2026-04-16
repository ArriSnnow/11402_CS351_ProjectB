cmake_minimum_required(VERSION 3.10)
project(CS351_ProjectB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable (placeholder until src files are added)
add_executable(csv_db src/main.cpp)

# Include directories
target_include_directories(csv_db PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable testing
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)
cmake_minimum_required(VERSION 3.14)
project(dacite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/.build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/.bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/.bin)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_executable(dacite ${CMAKE_SOURCE_DIR}/src/main.cpp ${SOURCES})

# Add test executable
file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/tests/*.cpp)
add_executable(lexer_test ${TEST_SOURCES} ${SOURCES})
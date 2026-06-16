cmake_minimum_required(VERSION 3.16)
project(event_recorder
    VERSION 1.0
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(event_recorder
    src/record_table_config.cpp
    src/record_table_generic.cpp
)

# Include directories for the library
target_include_directories(event_recorder
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(example)
add_subdirectory(test)

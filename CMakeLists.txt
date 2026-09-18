# TBD: Build tests and examples only if this is the top-level project.
# if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
#     # We are the top-level project.
# else()
#     # We are being included via add_subdirectory().
# endif()
#

cmake_minimum_required(VERSION 3.16)
project(event_recorder
    VERSION 1.0
    LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

function(enable_coverage target)
    target_compile_options(${target} PRIVATE --coverage)
    target_link_options(${target} PRIVATE --coverage)
endfunction()

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

add_library(event_recorder
    src/record_table_init_config.cpp
    src/record_table_manager.cpp
)
# Alias allows clients to use namespace for both embedded and installed builds.
add_library(event_recorder::event_recorder ALIAS event_recorder)

# Include directories for the library
target_include_directories(event_recorder
    PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
       $<INSTALL_INTERFACE:include>
)

install(DIRECTORY include/ DESTINATION include)
# Install the library and create package 'export set' descriptor event_recorderTargets.
install(TARGETS event_recorder EXPORT event_recorderTargets)
# Write package 'export set' to a file. It allows clients to import the package.
install(EXPORT event_recorderTargets
        FILE event_recorderTargets.cmake
        NAMESPACE event_recorder::
        DESTINATION lib/cmake/event_recorder)
# Write package configuration file; it allows clients to find the package descriptor file.
install(FILES cmake/event_recorderConfig.cmake
        DESTINATION lib/cmake/event_recorder)

add_subdirectory(example)
add_subdirectory(example_early)
add_subdirectory(test)

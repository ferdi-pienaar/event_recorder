

find_package(GTest REQUIRED)

add_executable(record_table_test
    record_table_test.cpp
)

target_link_libraries(record_table_test
    PRIVATE
        record_table
        GTest::gtest
        GTest::gtest_main
)

# Enable testing (if you want to use CTest)
#enable_testing()
#add_test(
#    NAME record_table_test
#   COMMAND record_table_test
#)

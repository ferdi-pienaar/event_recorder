
find_package(GTest REQUIRED)

add_executable(event_recorder_test
    event_recorder_test.cpp
)

target_link_libraries(event_recorder_test
    PRIVATE
        event_recorder
        GTest::gtest
        GTest::gtest_main
)

# Enable testing (if you want to use CTest)
#enable_testing()
#add_test(
#   NAME event_recorder_test
#   COMMAND event_recorder_test
#)

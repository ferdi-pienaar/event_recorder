
add_executable(event_recorder_example
    record_table_operator.cpp
    record_table_helper.cpp
    event_generator.cpp
)

target_link_libraries(event_recorder_example
    PRIVATE
        event_recorder
)


add_executable(record_table_example
    record_table_operator.cpp
    record_table_helper.cpp
    record_table_recorder.cpp
)

target_link_libraries(record_table_example
    PRIVATE
        record_table
)

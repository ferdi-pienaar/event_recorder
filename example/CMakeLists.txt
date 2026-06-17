
add_executable(event_recorder_example
    operator.cpp
    operator_helper.cpp
    event_generator.cpp
)

target_link_libraries(event_recorder_example
    PRIVATE
        event_recorder
)

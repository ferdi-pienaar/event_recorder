add_executable(event_recorder_example_early
    composition_root.cpp
    operator.cpp
    operator_helper.cpp
    event_generator.cpp
)

target_link_libraries(event_recorder_example_early
    PRIVATE
        event_recorder
)

if(ENABLE_COVERAGE)
    enable_coverage(event_recorder_example_early)
endif()

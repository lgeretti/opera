include(CTest)

set(UNIT_TESTS
    test_geometry
    test_body
    test_state
    test_packet
    test_serialisation
    test_deserialisation
    test_barrier
    test_scenarios
)

foreach(TEST ${UNIT_TESTS})
    add_executable(${TEST} ${TEST}.cpp)
    target_link_libraries(${TEST} opera)
    add_test(${TEST} ${TEST})
endforeach()

add_custom_target(tests)
add_dependencies(tests ${UNIT_TESTS})

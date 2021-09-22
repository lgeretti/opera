set(SCENARIOS
        scenario_nocollision
        scenario_nocollision_ws
    )

foreach(SCENARIO ${SCENARIOS})
    add_executable(${SCENARIO} ${SCENARIO}.cpp)
    target_link_libraries(${SCENARIO} opera)
endforeach()

add_custom_target(scenarios)
add_dependencies(scenarios ${SCENARIOS})
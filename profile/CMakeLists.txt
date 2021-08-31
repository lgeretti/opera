set(PROFILE_FILES
    profile_geometry
    profile_body
    profile_state
    profile_verification
)

foreach(PROFILE ${PROFILE_FILES})
    add_executable(${PROFILE} ${PROFILE}.cpp)
    target_link_libraries(${PROFILE} OPERA_SRC ${ARIADNE_LIBRARIES} ${LibRDKafka_LIBRARIES})
endforeach()

add_custom_target(profiles)
add_dependencies(profiles ${PROFILE_FILES})

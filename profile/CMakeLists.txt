set(PROFILE_FILES
    profile_algebra
    profile_body
)

foreach(PROFILE ${PROFILE_FILES})
    add_executable(${PROFILE} ${PROFILE}.cpp)
    target_link_libraries(${PROFILE} OPERA_SRC ${ARIADNE_LIBRARIES})
endforeach()

add_custom_target(profiles)
add_dependencies(profiles ${PROFILE_FILES})

add_library(OPERA_SRC OBJECT
   body.cpp
)

if(COVERAGE)
    target_link_libraries(OPERA_SRC PUBLIC coverage_config)
endif()
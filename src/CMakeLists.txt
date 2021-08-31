add_library(OPERA_SRC OBJECT
   geometry.cpp
   body.cpp
   serialisation.cpp
   deserialisation.cpp
   verification.cpp
)

if(COVERAGE)
    target_link_libraries(OPERA_SRC PUBLIC coverage_config)
endif()
add_library(OPERA_SRC OBJECT
   thread.cpp
   logging.cpp
   command_line_interface.cpp
   geometry.cpp
   body.cpp
   packet.cpp
   state.cpp
   serialisation.cpp
   deserialisation.cpp
   memory.cpp
   mqtt.cpp
   barrier.cpp
   verification.cpp
)

if(COVERAGE)
    target_link_libraries(OPERA_SRC coverage_config)
endif()
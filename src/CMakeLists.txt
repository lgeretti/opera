add_library(OPERA_SRC OBJECT
   geometry.cpp
   body.cpp
   packet.cpp
   state.cpp
   serialisation.cpp
   deserialisation.cpp
   broker.cpp
   memory.cpp
   kafka.cpp
   barrier.cpp
   verification.cpp
   websocket.cpp
)

if(COVERAGE)
    target_link_libraries(OPERA_SRC coverage_config)
endif()
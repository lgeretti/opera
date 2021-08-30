project(testKafka)
cmake_minimum_required(VERSION 3.0)


add_executable(testProducer testProducer.cpp)
target_link_libraries(testProducer PUBLIC rdkafka++)

<<<<<<< HEAD
find_package(Ariadne REQUIRED)
find_package(rapidjson REQUIRED)
find_package(LibRDKafka REQUIRED)
find_package(Threads REQUIRED)

set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/resources/)
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.hpp.cmake ${PROJECT_SOURCE_DIR}/include/config.hpp)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include ${ARIADNE_INCLUDE_DIRS} ${LibRDKafka_INCLUDE_DIR} ${RAPIDJSON_INCLUDEDIR})

add_subdirectory(src)
add_subdirectory(profile)
add_subdirectory(scenarios)

add_library(opera SHARED $<TARGET_OBJECTS:OPERA_SRC>)
target_link_libraries(opera ${ARIADNE_LIBRARIES} ${LibRDKafka_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

add_executable(operad src/operad.cpp)
target_link_libraries(operad opera)

option(COVERAGE "Enable coverage reporting" OFF)
add_library(coverage_config INTERFACE)
if(COVERAGE)
    message(STATUS "Enabled coverage reporting")
    target_compile_options(coverage_config INTERFACE -O0 -g --coverage)
    if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.13)
        target_link_options(coverage_config INTERFACE --coverage)
    else()
        target_link_libraries(coverage_config INTERFACE --coverage)
    endif()
    target_link_libraries(opera coverage_config)
endif()

enable_testing()
add_subdirectory(test)

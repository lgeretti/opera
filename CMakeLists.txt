cmake_minimum_required(VERSION 3.9)

project(Opera VERSION 1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Ariadne REQUIRED)
find_package(rapidjson REQUIRED)
find_package(LibRDKafka REQUIRED)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include ${ARIADNE_INCLUDE_DIRS} ${LibRDKafka_INCLUDE_DIR} ${RAPIDJSON_INCLUDEDIR})

add_subdirectory(src)
add_subdirectory(profile)

add_executable(opera src/opera.cpp)

target_link_libraries(opera OPERA_SRC ${ARIADNE_LIBRARIES} ${LibRDKafka_LIBRARIES})

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
endif()

enable_testing()
add_subdirectory(test)



cmake_minimum_required(VERSION 3.9)

project(Opera VERSION 1.0)

if(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    message(WARNING "Intel compiler not supported.")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(WARNING "Microsoft compiler not supported.")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.2)
        message(FATAL_ERROR "GCC version must be at least 10.2!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "Clang version must be at least 11.0!")
    endif()
else()
    message(WARNING "You are using an unidentified compiler! Compilation has only been tested with Clang and GCC.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ANY_TARGET_WARN all extra pedantic sign-conversion cast-qual disabled-optimization
        init-self missing-include-dirs sign-promo switch-default undef redundant-decls
        strict-aliasing unused-parameter shadow error)
set(ANY_TARGET_DONT_WARN strict-aliasing unused-parameter)
set(LIBRARY_EXCLUSIVE_WARN shadow)

foreach(WARN ${ANY_TARGET_WARN})
    add_compile_options(-W${WARN})
endforeach()

find_package(rapidjson REQUIRED)
find_package(Threads REQUIRED)
find_package(Mosquitto REQUIRED)

set(RESOURCES_PATH ${PROJECT_SOURCE_DIR}/resources/)
configure_file(${PROJECT_SOURCE_DIR}/cmake/config.hpp.cmake ${PROJECT_SOURCE_DIR}/include/config.hpp)

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/include ${Mosquitto_INCLUDE_DIR} ${RAPIDJSON_INCLUDEDIR})

add_subdirectory(src)
add_subdirectory(profile)
add_subdirectory(scenarios)

add_library(opera SHARED $<TARGET_OBJECTS:OPERA_SRC>)
target_link_libraries(opera ${Mosquitto_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

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

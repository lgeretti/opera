cmake_minimum_required(VERSION 3.9)

project(Opera VERSION 1.0)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 19.20)
        message(FATAL_ERROR "MSVC version must be at least 19.20!")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.2)
        message(FATAL_ERROR "GCC version must be at least 10.2!")
    endif()
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0)
        message(FATAL_ERROR "Clang version must be at least 11.0!")
    endif()
else()
    message(WARNING "You are using an unsupported compiler! MSVC, GCC and Clang are supported.")
endif()

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set(LIBRARY_KIND STATIC)
else()
    set(LIBRARY_KIND SHARED)	
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MACOSX_RPATH 1)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
  set(ANY_TARGET_WARN all extra pedantic sign-conversion cast-qual disabled-optimization
        init-self missing-include-dirs sign-promo switch-default undef redundant-decls
        strict-aliasing unused-parameter shadow error)
  foreach(WARN ${ANY_TARGET_WARN})
    add_compile_options(-W${WARN})
  endforeach()
endif()

find_package(mqtt REQUIRED)
find_package(Threads REQUIRED)

include_directories(SYSTEM ${mqtt_INCLUDE_DIR})

add_executable(opera src/opera.cpp)
target_link_libraries(opera ${mqtt_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

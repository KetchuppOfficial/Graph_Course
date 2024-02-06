cmake_minimum_required(VERSION 3.15)

project(Graph
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(GTest REQUIRED)
enable_testing()

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(test)

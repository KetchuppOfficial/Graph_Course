cmake_minimum_required(VERSION 3.15)

project(Graph
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

set(CMAKE_CXX_STANDARD          20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_executable(graph ./main.cpp)

target_include_directories(graph
                           PRIVATE ${INCLUDE_DIR})

cmake_minimum_required(VERSION 3.15)

project(Graph
        LANGUAGES CXX)

if (${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build is forbidden")
endif()

find_package(GTest REQUIRED)
enable_testing()

find_package(Threads REQUIRED)
find_package(Boost REQUIRED
             COMPONENTS HEADERS PROGRAM_OPTIONS)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/test/end-to-end)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

add_subdirectory(test)

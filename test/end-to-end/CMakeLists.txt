add_executable(driver ./src/driver.cpp)
add_executable(recursive_driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)
add_executable(graphic_dump ./src/driver.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

target_include_directories(recursive_driver
                           PRIVATE ${INCLUDE_DIR})

target_include_directories(graphic_dump
                           PRIVATE ${INCLUDE_DIR})

target_compile_definitions(driver
                           PRIVATE DFS)

target_compile_definitions(recursive_driver
                           PRIVATE DFS RECURSIVE)

target_compile_definitions(graphic_dump
                           PRIVATE GRAPHIC)

target_link_libraries(generator
                      Boost::program_options)

install(TARGETS driver driver recursive_driver generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

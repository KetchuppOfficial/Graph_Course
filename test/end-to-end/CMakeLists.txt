add_executable(driver ./src/driver.cpp)
add_executable(recursive_driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

target_include_directories(recursive_driver
                           PRIVATE ${INCLUDE_DIR})

target_compile_definitions(recursive_driver
                           PRIVATE RECURSIVE)

target_link_libraries(generator
                      Boost::program_options)

install(TARGETS driver driver recursive_driver generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

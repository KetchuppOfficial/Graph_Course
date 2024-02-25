add_executable(driver ./src/driver.cpp)
add_executable(recursive_driver ./src/driver.cpp)
add_executable(generator ./src/generator.cpp)

target_include_directories(driver
                           PRIVATE ${INCLUDE_DIR})

target_include_directories(recursive_driver
                           PRIVATE ${INCLUDE_DIR})

target_compile_definitions(recursive_driver
                           PRIVATE RECURSIVE)

install(TARGETS driver driver recursive_driver generator
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

add_executable(driver ./src/driver.cpp)
target_include_directories(driver PRIVATE ${INCLUDE_DIR})
target_compile_definitions(driver PRIVATE TEST_DFS)

add_executable(recursive_driver ./src/driver.cpp)
target_include_directories(recursive_driver PRIVATE ${INCLUDE_DIR})
target_compile_definitions(recursive_driver PRIVATE TEST_DFS RECURSIVE)

add_executable(generator ./src/generator.cpp)
target_link_libraries(generator PRIVATE Boost::program_options)

add_executable(graphic_dump ./src/driver.cpp)
target_include_directories(graphic_dump PRIVATE ${INCLUDE_DIR})
target_compile_definitions(graphic_dump PRIVATE GRAPHIC)

install(TARGETS driver recursive_driver generator graphic_dump
        DESTINATION ${INSTALL_DIR}/directed-graph)

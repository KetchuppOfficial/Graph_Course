add_executable(kgraph-driver ./src/driver.cpp)
target_include_directories(kgraph-driver PRIVATE ${INCLUDE_DIR})

install(TARGETS kgraph-driver
        DESTINATION ${INSTALL_DIR}/kgraph)

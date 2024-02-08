cmake_minimum_required(VERSION 3.10)
project(KPABE_CF)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

# Specify RELIC and LSSS include directories
# include_directories(
#     /usr/local/include/relic
#     /usr/local/include/lsss
# )


file(
    GLOB_RECURSE
    SOURCE_FILES
    dpvs/*
    keys/*
    kpabe/*
    utils.c
    utils.h
)

add_executable(
    kpabe_out
    ${SOURCE_FILES}
    main.cpp
)

add_executable(
    example_out
    ${SOURCE_FILES}
    examples/examples.cpp
)

add_executable(
    bench_out
    ${SOURCE_FILES}
    bench/bench.cpp
)

target_link_libraries(kpabe_out relic_s lsss gmp)
target_link_libraries(example_out relic_s lsss gmp)
target_link_libraries(bench_out relic_s lsss gmp)

add_custom_target(example
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target example_out
    COMMENT "Building example_out executable"
)

add_custom_target(bench
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target bench_out
    COMMENT "Building bench_out executable"
)

# Install dependencies
add_custom_target(install_dependencies_local
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts local
    COMMENT "Installing dependencies locally"
)

add_custom_target(install_dependencies_docker
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts docker
    COMMENT "Installing dependencies in Docker"
)

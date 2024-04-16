cmake_minimum_required(VERSION 3.10)
project(KPABE_CF)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CXXFLAGS "-Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")


# file(
#     GLOB_RECURSE
#     SOURCE_FILES
#     dpvs/*
#     serializer/serializer.cpp
#     keys/*
#     kpabe/*
#     utils.c
#     utils.h
# )

set(SOURCE_FILES
    dpvs/matrix.c
    dpvs/dpvs.c
    dpvs/vector_ec.cpp
    keys/keys.cpp
    kpabe/kpabe.cpp
    serializer/serializer.cpp
    utils.c
    utils.h
)

add_executable(
    kpabe_out
    ${SOURCE_FILES}
    main.cpp
)

# add_executable(
#     example_out
#     ${SOURCE_FILES}
#     examples/examples.cpp
# )

add_executable(
    new_examples
    ${SOURCE_FILES}
    examples/new_examples.cpp
)

add_executable(
    new_bench
    ${SOURCE_FILES}
    bench/new_bench.cpp
)

target_link_libraries(kpabe_out lsss relic_s gmp)
target_link_libraries(new_examples lsss relic_s gmp)
# target_link_libraries(example_out relic_s lsss gmp)
target_link_libraries(new_bench lsss relic_s gmp)


add_custom_target(example
    # COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target example_out
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target new_examples
    COMMENT "Building example_out executable"
)

add_custom_target(bench
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target new_bench
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

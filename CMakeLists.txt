cmake_minimum_required(VERSION 3.10)
project(KPABE_CF)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CXXFLAGS "-Wall -Wextra -Wno-unused-parameter")

# By default, compression is enabled
option(COMPRESSION_ENABLED "Enable compression" ON)

if(COMPRESSION_ENABLED)
    set(_COMPRESSION_ true)
else()
    set(_COMPRESSION_ false)
endif()

# set value of _COMPRESSION_ in CXXFLAGS
set(CMAKE_CXX_FLAGS "${CXXFLAGS} -D_COMPRESSION_=${_COMPRESSION_}")

set(SOURCE_FILES
    dpvs/matrix.c
    dpvs/dpvs.c
    dpvs/vector_ec.cpp
    keys/keys.cpp
    kpabe/kpabe.cpp
    serializer/serializer.cpp
    utils.c
)

add_executable(
    kpabe_out
    ${SOURCE_FILES}
    main.cpp
)

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
target_link_libraries(new_bench lsss relic_s gmp)


add_custom_target(example
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target new_examples
    COMMENT "Building example_out executable"
)

add_custom_target(bench
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target new_bench
    COMMENT "Building bench_out executable"
)

# Install dependencies
add_custom_target(install_dependencies_local
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts COMPRESSION=${_COMPRESSION_} local
    COMMENT "Installing dependencies locally"
)

add_custom_target(install_dependencies_docker
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts COMPRESSION=${_COMPRESSION_} docker
    COMMENT "Installing dependencies in Docker"
)

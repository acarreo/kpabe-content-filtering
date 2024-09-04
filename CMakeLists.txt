cmake_minimum_required(VERSION 3.10)
project(KPABE_CF)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CXXFLAGS "-Wall -Wextra -Wno-unused-parameter")

# Default parameters
set(CURVE "bls12-381" CACHE STRING "Elliptic curve to use")
set(RELIC_TAG "0.6.0" CACHE STRING "Relic tag to use")
set(LSSS_IMAGE_VERSION "0.1.0" CACHE STRING "LSSS image version to use")
set(COMPRESSION ON CACHE BOOL "Enable compression")

# Print the options
message(STATUS "Elliptic Curve: ${CURVE}")
message(STATUS "Relic Tag: ${RELIC_TAG}")
message(STATUS "LSSS Image Version: ${LSSS_IMAGE_VERSION}")
message(STATUS "Enable compression: ${COMPRESSION}")

if(COMPRESSION)
    set(LSSS_LIBRARY "lsss_abe_compression")
else()
    set(LSSS_LIBRARY "lsss_abe_no_compression")
endif()

find_package(OpenSSL REQUIRED)
find_library(RLC_LIBRARY NAMES relic)

set(LIBRARIES
    OpenSSL::SSL
    ${LSSS_LIBRARY}
    ${RLC_LIBRARY}
    gmp
)

# Include directories
include_directories(
    /usr/local/include
    /usr/local/include/lsss_abe
)

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

target_link_libraries(kpabe_out ${LIBRARIES})
target_link_libraries(new_examples ${LIBRARIES})
target_link_libraries(new_bench ${LIBRARIES})

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
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts ENABLE_COMPRESSION=${COMPRESSION} ELLIPTIC_CURVE=${CURVE} local
    COMMENT "Installing dependencies locally"
)

add_custom_target(install_dependencies_docker
    COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/scripts RELIC_TAG=${RELIC_TAG} ELLIPTIC_CURVE=${CURVE} LSSS_IMAGE_VERSION=${LSSS_IMAGE_VERSION} ENABLE_COMPRESSION=${COMPRESSION} docker
    COMMENT "Installing dependencies in Docker"
)

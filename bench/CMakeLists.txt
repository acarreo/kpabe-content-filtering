cmake_minimum_required(VERSION 3.10)
project(KP-ABE--Benchmark)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(RLC_LIBRARY NAMES relic)

# By default, compression is enabled
option(ENABLE_COMPRESSION "Enable compression" ON)

if(ENABLE_COMPRESSION STREQUAL "ON")
    message(STATUS "Compression enabled")
    set(LSSS_LIBRARY "lsss_abe_compression")
elseif(ENABLE_COMPRESSION STREQUAL "OFF")
    message(STATUS "Compression disabled")
    set(LSSS_LIBRARY "lsss_abe_no_compression")
else()
    message(FATAL_ERROR "Invalid value for ENABLE_COMPRESSION. Please use ON or OFF")
endif()

set(LIBRARIES
    benchmark::benchmark
    OpenSSL::SSL
    ${LSSS_LIBRARY}
    ${RLC_LIBRARY}
    gmp
)

# Include directories
include_directories(/usr/local/include/lsss_abe)

set(SOURCE_FILES
  ../dpvs/matrix.c
  ../dpvs/dpvs.c
  ../dpvs/vector_ec.cpp
  ../keys/keys.cpp
  ../kpabe/kpabe.cpp
  ../serializer/serializer.cpp
  ../utils.c
  bench-utils.cpp
)

add_executable(
  bench_setup_efficiency
  ${SOURCE_FILES}
  bench--setup--efficiency.cpp
)

add_executable(
  bench_setup_serialization
  ${SOURCE_FILES}
  bench--setup--serialization.cpp
)

add_executable(
  bench_keygen_efficiency
  ${SOURCE_FILES}
  bench--keygen--efficiency.cpp
)

add_executable(
  bench_keygen_serialization
  ${SOURCE_FILES}
  bench--keygen--serialization.cpp
)

add_executable(
  bench_encrypt_efficiency
  ${SOURCE_FILES}
  bench--encrypt--efficiency.cpp
)

add_executable(
  bench_encrypt_serialization
  ${SOURCE_FILES}
  bench--encrypt--serialization.cpp
)

add_executable(
  bench_decrypt_efficiency
  ${SOURCE_FILES}
  bench--decrypt--efficiency.cpp
)

target_link_libraries(bench_setup_efficiency ${LIBRARIES})
target_link_libraries(bench_setup_serialization ${LIBRARIES})
target_link_libraries(bench_keygen_efficiency ${LIBRARIES})
target_link_libraries(bench_keygen_serialization ${LIBRARIES})
target_link_libraries(bench_encrypt_efficiency ${LIBRARIES})
target_link_libraries(bench_encrypt_serialization ${LIBRARIES})
target_link_libraries(bench_decrypt_efficiency ${LIBRARIES})


# Add a target to run all benchmarks
add_custom_target(run_benchmarks
  COMMAND bench_setup_efficiency
  COMMAND bench_setup_serialization
  COMMAND bench_keygen_efficiency
  COMMAND bench_keygen_serialization "serialize"
  COMMAND bench_keygen_serialization "deserialize"
  COMMAND bench_encrypt_efficiency
  COMMAND bench_encrypt_serialization "serialize"
  COMMAND bench_encrypt_serialization "deserialize"
  COMMAND bench_decrypt_efficiency
)

cmake_minimum_required(VERSION 3.10)
project(KP-ABE--Benchmark)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(RLC_LIBRARY NAMES relic)

set(LIBRARIES
    benchmark::benchmark
    OpenSSL::SSL
    /usr/local/lib/liblsss_abe_compression.so
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

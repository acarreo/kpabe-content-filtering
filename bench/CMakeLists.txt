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
include_directories(
  /usr/local/include
  /usr/local/include/lsss_abe
)

set(SOURCE_FILES
  ../dpvs/matrix.c
  ../dpvs/dpvs.c
  ../dpvs/vector_ec.cpp
  ../keys/keys.cpp
  ../kpabe/kpabe.cpp
  ../serializer/serializer.cpp
  ../utils.c
)

add_executable(
  bench_out
  ${SOURCE_FILES}
  bench.cpp
)

target_link_libraries(bench_out ${LIBRARIES})
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
    /usr/local/lib/liblsss_abe_no_compression.so
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
  bench_serialization
  ${SOURCE_FILES}
  bench-serialization.cpp
)

add_executable(
  bench_efficiency_primitives
  ${SOURCE_FILES}
  bench-efficiency-primitives.cpp
)

add_executable(
  bench_efficiency_dpvs
  ${SOURCE_FILES}
  bench-efficiency-dpvs.cpp
)

target_link_libraries(bench_serialization ${LIBRARIES})
target_link_libraries(bench_efficiency_primitives ${LIBRARIES})
target_link_libraries(bench_efficiency_dpvs ${LIBRARIES})

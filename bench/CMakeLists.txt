cmake_minimum_required(VERSION 3.10)
project(KP-ABE--Benchmark)

set(CMAKE_C_COMPILER cc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)

find_package(benchmark REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(RLC_LIBRARY NAMES relic)
find_library(LSSS_LIBRARY NAMES abe_lsss REQUIRED)

set(KPABE_LIBRARY kpabe_dpvs)

set(LIBRARIES
    benchmark::benchmark
    OpenSSL::SSL
    ${LSSS_LIBRARY}
    ${RLC_LIBRARY}
    gmp
)

################################################################################

macro (add_bench BENCH_NAME CUSTOM_TARGET_NAME BENCH_FILE)
  add_executable(${BENCH_NAME} ${BENCH_FILE} ${BENCH_UTILS_SOURCE_FILE})
  target_link_libraries(${BENCH_NAME} ${LIBRARIES} ${KPABE_LIBRARY})
  target_include_directories(${BENCH_NAME}
    PRIVATE /usr/local/include/abe_lsss /usr/local/include/kpabe_dpvs)
  add_custom_target(${CUSTOM_TARGET_NAME} COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${BENCH_NAME})
endmacro()

# Function to create a custom command for benchmarking efficiency
function(add_benchmark_target target_name)
  add_custom_command(
    OUTPUT run_${target_name}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target_name}
            --benchmark_out=${CMAKE_CURRENT_SOURCE_DIR}/results/${target_name}.json
            --benchmark_time_unit=ms
            # --benchmark_repetitions=5
            --benchmark_counters_tabular=true
    DEPENDS ${target_name}
    COMMENT "Running ${target_name} benchmark"
  )
  add_custom_target(${target_name}_efficiency DEPENDS run_${target_name})
endfunction()

# Function to create a custom command for benchmarking serialization tasks
function(add_benchmark_serialization target_name mode)
  if(NOT mode STREQUAL "serialize" AND NOT mode STREQUAL "deserialize")
    if(NOT mode)
      set(mode "serialize")
    else()
      message(WARNING
        "add_benchmark_serialization: invalid mode '${mode}', defaulting to 'serialize'"
      )
      set(mode "serialize")
    endif()
  endif()

  add_custom_command(
    OUTPUT run_${target_name}${mode}
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target_name}
            ${mode}
            --benchmark_out=${CMAKE_CURRENT_SOURCE_DIR}/results/${target_name}${mode}.json
            --benchmark_time_unit=ms
            # --benchmark_repetitions=5
            --benchmark_counters_tabular=true
    DEPENDS ${target_name}
    COMMENT "Running ${target_name} benchmark (${mode})"
  )

  add_custom_target(${target_name}${mode} DEPENDS run_${target_name}${mode})
endfunction()


set(BENCH_UTILS_SOURCE_FILE "bench-utils.cpp")

# Efficiency benchmarks
add_bench(bench_setup setup bench--setup--efficiency.cpp)
add_bench(bench_keygen keygen bench--keygen--efficiency.cpp)
add_bench(bench_encrypt encrypt bench--encrypt--efficiency.cpp)
add_bench(bench_decrypt decrypt bench--decrypt--efficiency.cpp)

# Serialization benchmarks
add_bench(bench_setup_ setup_ bench--setup--serialization.cpp)
add_bench(bench_keygen_ keygen_ bench--keygen--serialization.cpp)
add_bench(bench_encrypt_ encrypt_ bench--encrypt--serialization.cpp)


# Create custom commands for each benchmark
add_benchmark_target(bench_setup)
add_benchmark_target(bench_keygen)
add_benchmark_target(bench_encrypt)
add_benchmark_target(bench_decrypt)

# Create custom commands for serialization benchmarks
add_benchmark_serialization(bench_setup_ "")
add_benchmark_serialization(bench_keygen_ "serialize")
add_benchmark_serialization(bench_keygen_ "deserialize")
add_benchmark_serialization(bench_encrypt_ "serialize")
add_benchmark_serialization(bench_encrypt_ "deserialize")

# Add a custom target to run all benchmarks
add_custom_target(benchmarks
  DEPENDS bench_setup_efficiency
          bench_keygen_efficiency
          bench_encrypt_efficiency
          bench_decrypt_efficiency
          bench_setup_efficiency
          bench_keygen_serialize
          bench_keygen_deserialize
          bench_encrypt_serialize
          bench_encrypt_deserialize
)

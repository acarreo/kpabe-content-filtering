#include <benchmark/benchmark.h>
#include "bench.hpp"

using namespace std;

static void BM_DPVS_Generation(benchmark::State& state, uint8_t dim) {
  for (auto _ : state) {
    if (!dpvs_generate_bases(dim)) {
      cerr << "Error: Could not generate DPVS bases" << endl;
      exit(1);
    }
  }
}

static void BM_InnerProduct(benchmark::State& state, uint8_t dim) {
  G1_VECTOR x; x.random(dim);
  G2_VECTOR y; y.random(dim);
  for (auto _ : state) {
    auto res = innerProduct(x, y);
  }
}

BENCHMARK_CAPTURE(BM_DPVS_Generation, DPVS_Generation_1, 1)->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(BM_DPVS_Generation, DPVS_Generation_5, 5)->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(BM_DPVS_Generation, DPVS_Generation_10, 10)->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(BM_DPVS_Generation, DPVS_Generation_100, 100)->Unit(benchmark::kMicrosecond);

BENCHMARK_CAPTURE(BM_InnerProduct, InnerProduct_10, 10)->Unit(benchmark::kMicrosecond);
BENCHMARK_CAPTURE(BM_InnerProduct, InnerProduct_100, 100)->Unit(benchmark::kMicrosecond);

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}

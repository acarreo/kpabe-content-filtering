#include <benchmark/benchmark.h>
#include "bench.hpp"

using namespace std;

static void BM_KPABE_DPVS_Setup(benchmark::State& state) {
  for (auto _ : state) {
    KPABE_DPVS kpabe;
    if (!kpabe.setup()) {
      cerr << "Error: Could not setup KPABE_DPVS" << endl;
      exit(1);
    }
  }
}
BENCHMARK(BM_KPABE_DPVS_Setup)->Unit(benchmark::kMillisecond);


int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}
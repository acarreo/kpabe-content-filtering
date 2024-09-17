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

static void BM_KPABE_DPVS_PublicKeyRandomization(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto pk = kpabe.get_public_key();
  for (auto _ : state) {
    auto __pair = pk.randomize();
  }
}

static void BM_KPABE_DPVS_ValidateDerivedPublicKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto pk = kpabe.get_public_key();
  auto __pair = pk.randomize();
  for (auto _ : state) {
    if (!pk.validate_derived_key(__pair.first, __pair.second)) {
      cerr << "Error: Could not validate public key" << endl;
      exit(1);
    }
  }
}

BENCHMARK(BM_KPABE_DPVS_Setup)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_PublicKeyRandomization)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_ValidateDerivedPublicKey)->Unit(benchmark::kMicrosecond);

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}
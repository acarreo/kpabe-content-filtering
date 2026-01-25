#include <benchmark/benchmark.h>
#include <string>
#include <fstream>
#include <iomanip>
#include "bench.hpp"

using namespace std;


static void BM_KPABE_DPVS_Encrypt(benchmark::State& state, int nb_attributes) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  uint8_t ss_key[RLC_MD_LEN];
  auto public_key = kpabe.get_public_key();
  auto attributes = generateAttributes(nb_attributes);
  std::string url = "www.example.com";

  for (auto _ : state) {
    KPABE_DPVS_CIPHERTEXT ctx(attributes, url);
    ctx.encrypt(ss_key, public_key);
  }

  // Set the custom value for size
  state.counters["Nb_Attributes"] = nb_attributes;
}


int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  std::vector<int> nb_attributes_list = {1, 10, 100, 1000};

  for (auto n_att : nb_attributes_list) {
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_Encrypt", [n_att](benchmark::State& state) {
      BM_KPABE_DPVS_Encrypt(state, n_att);
    })->Unit(benchmark::kMillisecond);
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}

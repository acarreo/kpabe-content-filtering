#include <benchmark/benchmark.h>
#include <string>
#include <iomanip>
#include <fstream>

#include "bench.hpp"

using namespace std;

static void BM_KPABE_DPVS_DecryptionKeyGeneration(benchmark::State& state, policy_params params) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto wl = generateAttributesList("wl_url_", params.nwl);
  auto bl = generateAttributesList("wl_url_", params.nbl);

  for (auto _ : state) {
    auto dec_key = kpabe.keygen(params.policy, wl, bl);
    if (!dec_key) {
      cerr << "Error: Could not generate keys" << endl;
      exit(1);
    }
  }

  // Set the custom value for size
  state.counters["Nb_WL"] = params.nwl;
  state.counters["Nb_BL"] = params.nbl;
}


string policy = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

int main(int argc, char** argv) {

  InitializeOpenABE();

  __relic_print_params();

  std::vector<int> nb_attributes_list = {0, 1, 10, 100, 1000};

  for (auto nwl : nb_attributes_list) {
    policy_params params = {nwl, 0, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    });
  }

  for (auto nbl : nb_attributes_list) {
    policy_params params = {0, nbl, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    });
  }

  for (auto nb : nb_attributes_list) {
    policy_params params = {nb, nb, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    });
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  ShutdownOpenABE();

  return 0;
}

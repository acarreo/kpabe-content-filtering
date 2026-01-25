#include <benchmark/benchmark.h>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>

#include "bench.hpp"

using namespace std;

static void BM_KPABE_DPVS_SerializeDecryptionKey(benchmark::State& state, policy_params params) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto wl = generateAttributesList("WL_url_", params.nwl);
  auto bl = generateAttributesList("BL_url_", params.nbl);
  auto dec_key = kpabe.keygen(params.policy, wl, bl);

  if (!dec_key) {
    cerr << "Error: Could not generate keys" << endl;
    exit(1);
  }

  for (auto _ : state) {
    OpenABEByteString dec_key_bytes;
    dec_key->serialize(dec_key_bytes);
  }

  // Serialize the decryption key once to get the size
  OpenABEByteString dec_key_bytes;
  dec_key->serialize(dec_key_bytes);

  // Set the custom value for size
  state.counters["Size"] = dec_key_bytes.size();
  state.counters["Nb_WL"] = params.nwl;
  state.counters["Nb_BL"] = params.nbl;
}

static void BM_KPABE_DPVS_DeserializeDecryptionKey(benchmark::State& state, policy_params params) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto wl = generateAttributesList("WL_url_", params.nwl);
  auto bl = generateAttributesList("BL_url_", params.nbl);
  auto dec_key = kpabe.keygen(params.policy, wl, bl);

  if (!dec_key) {
    cerr << "Error: Could not generate keys" << endl;
    exit(1);
  }

  OpenABEByteString dec_key_bytes;
  dec_key->serialize(dec_key_bytes);
  for (auto _ : state) {
    KPABE_DPVS_DECRYPTION_KEY dec_key_deserialized;
    dec_key_deserialized.deserialize(dec_key_bytes);

    // Check if deserialized decryption key is correct
    // if (!(dec_key_deserialized == *dec_key)) {
    //   cerr << "Error: Deserialized decryption key is incorrect" << endl;
    //   exit(1);
    // }
  }

  // Set the custom value for size
  state.counters["Size"] = dec_key_bytes.size();
  state.counters["Nb_WL"] = params.nwl;
  state.counters["Nb_BL"] = params.nbl;
}


string policy = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  std::vector<std::pair<int, int>> ListSizes = {
    {0, 0}, {0, 10}, {0, 100}, {0, 1000},
    {10, 0}, {10, 10}, {10, 100}, {10, 1000},
    {100, 0}, {100, 10}, {100, 100}, {100, 1000},
    {1000, 0}, {1000, 10}, {1000, 100}, {1000, 1000}
  };

  for (auto num : ListSizes) {
    policy_params params = {num.first, num.second, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_SerializeDecryptionKey", [params](benchmark::State& state) {
      BM_KPABE_DPVS_SerializeDecryptionKey(state, params);
    });
  }

  for (auto num : ListSizes) {
    policy_params params = {num.first, num.second, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DeserializeDecryptionKey", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DeserializeDecryptionKey(state, params);
    });
  }

  // Run benchmark
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}

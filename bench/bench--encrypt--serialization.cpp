#include <benchmark/benchmark.h>
#include <string>
#include <fstream>

#include "bench.hpp"

using namespace std;


static void BM_KPABE_DPVS_SerializationCiphertext(benchmark::State& state, int nb_attributes) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  uint8_t ss_key[RLC_MD_LEN];
  KPABE_DPVS_CIPHERTEXT ctx(generateAttributes(nb_attributes), "www.example.com");
  ctx.encrypt(ss_key, kpabe.get_public_key());

  ByteString ctx_bytes;
  for (auto _ : state) {
    ctx.serialize(ctx_bytes);
  }

  // Set the custom value for size
  state.counters["Nb_Attributes"] = nb_attributes;
  state.counters["Size"] = ctx_bytes.size();
}

static void BM_KPABE_DPVS_DeserializationCiphertext(benchmark::State& state, int nb_attributes) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  uint8_t ss_key[RLC_MD_LEN];
  KPABE_DPVS_CIPHERTEXT ctx(generateAttributes(nb_attributes), "www.example.com");
  ctx.encrypt(ss_key, kpabe.get_public_key());

  ByteString ctx_bytes;
  ctx.serialize(ctx_bytes);
  for (auto _ : state) {
    KPABE_DPVS_CIPHERTEXT ctx_deserialized;
    ctx_deserialized.deserialize(ctx_bytes);
  }

  // Set the custom value for size
  state.counters["Nb_Attributes"] = nb_attributes;
  state.counters["Size"] = ctx_bytes.size();
}


int main(int argc, char** argv) {

  InitializeOpenABE();

  __relic_print_params();

  std::vector<int> nb_attributes_list = {1, 10, 100, 1000};

  for (auto n_att : nb_attributes_list) {
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_SerializationCiphertext", [n_att](benchmark::State& state) {
      BM_KPABE_DPVS_SerializationCiphertext(state, n_att);
    });
  }

  for (auto n_att : nb_attributes_list) {
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DeserializationCiphertext", [n_att](benchmark::State& state) {
      BM_KPABE_DPVS_DeserializationCiphertext(state, n_att);
    });
  }

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  ShutdownOpenABE();

  return 0;
}

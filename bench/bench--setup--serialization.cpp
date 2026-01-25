#include <benchmark/benchmark.h>
#include <string>
#include <fstream>
#include <vector>

#include "bench.hpp"

using namespace std;


static void BM_KPABE_DPVS_SerializePublicKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto pk = kpabe.get_public_key();

  for (auto _ : state) {
    OpenABEByteString pk_bytes;
    pk.serialize(pk_bytes);
  }

  OpenABEByteString pk_bytes;
  pk.serialize(pk_bytes);

  state.counters["Size"] = pk_bytes.size();
}

static void BM_KPABE_DPVS_DeserializePublicKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  OpenABEByteString pk_bytes;
  kpabe.get_public_key().serialize(pk_bytes);
  for (auto _ : state) {
    KPABE_DPVS_PUBLIC_KEY pk_deserialized;
    pk_deserialized.deserialize(pk_bytes);

    // Check if deserialized public key is correct
    // if (!(pk_deserialized == kpabe.get_public_key())) {
    //   cerr << "Error: Deserialized public key is incorrect" << endl;
    //   exit(1);
    // }
  }

  state.counters["Size"] = pk_bytes.size();
}

static void BM_KPABE_DPVS_SerializeMasterKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto mk = kpabe.get_master_key();

  for (auto _ : state) {
    OpenABEByteString mk_bytes;
    mk.serialize(mk_bytes);
  }

  OpenABEByteString mk_bytes;
  mk.serialize(mk_bytes);

  state.counters["Size"] = mk_bytes.size();
}

static void BM_KPABE_DPVS_DeserializeMasterKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  OpenABEByteString mk_bytes;
  kpabe.get_master_key().serialize(mk_bytes);
  for (auto _ : state) {
    KPABE_DPVS_MASTER_KEY mk_deserialized;
    mk_deserialized.deserialize(mk_bytes);

    // Check if deserialized master key is correct
    // if (!(mk_deserialized == kpabe.get_master_key())) {
    //   cerr << "Error: Deserialized master key is incorrect" << endl;
    //   exit(1);
    // }
  }

  state.counters["Size"] = mk_bytes.size();
}

BENCHMARK(BM_KPABE_DPVS_SerializePublicKey);
BENCHMARK(BM_KPABE_DPVS_SerializeMasterKey);
BENCHMARK(BM_KPABE_DPVS_DeserializePublicKey);
BENCHMARK(BM_KPABE_DPVS_DeserializeMasterKey);

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}

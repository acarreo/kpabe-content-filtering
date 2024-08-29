#include <benchmark/benchmark.h>
#include <iostream>
#include "../kpabe/kpabe.hpp"

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
    if (!(pk_deserialized == kpabe.get_public_key())) {
      cerr << "Error: Deserialized public key is incorrect" << endl;
      exit(1);
    }
  }
}

static void BM_KPABE_DPVS_SerializeMasterKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    return;
  }

  auto mk = kpabe.get_master_key();
  for (auto _ : state) {
    OpenABEByteString mk_bytes;
    mk.serialize(mk_bytes);
  }
}

// BENCHMARK(BM_KPABE_DPVS_Setup)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_SerializePublicKey)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_SerializeMasterKey)->Unit(benchmark::kMicrosecond);

BENCHMARK(BM_KPABE_DPVS_DeserializePublicKey)->Unit(benchmark::kMicrosecond);

int main(int argc, char** argv) {

  // InitializeOpenABE();
  if (!init_libraries()) return 1;

  // Run benchmarks
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();

  // ShutdownOpenABE();
  clean_libraries();

  return 0;
}
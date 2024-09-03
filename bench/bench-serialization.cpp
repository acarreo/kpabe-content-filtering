#include <benchmark/benchmark.h>
#include <string>
#include <fstream>

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
    exit(1);
  }

  auto mk = kpabe.get_master_key();
  for (auto _ : state) {
    OpenABEByteString mk_bytes;
    mk.serialize(mk_bytes);
  }
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
    if (!(mk_deserialized == kpabe.get_master_key())) {
      cerr << "Error: Deserialized master key is incorrect" << endl;
      exit(1);
    }
  }
}

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
}


// BENCHMARK(BM_KPABE_DPVS_SerializePublicKey)->Unit(benchmark::kMicrosecond);
// BENCHMARK(BM_KPABE_DPVS_SerializeMasterKey)->Unit(benchmark::kMicrosecond);
// BENCHMARK(BM_KPABE_DPVS_DeserializePublicKey)->Unit(benchmark::kMicrosecond);
// BENCHMARK(BM_KPABE_DPVS_DeserializeMasterKey)->Unit(benchmark::kMicrosecond);


string policy_1 = "Attr_5 and ((Attr_1 and Attr_2) or (Attr_3 and Attr_4))";
string policy_2 = "Attr_5 and ((Attr_1 and Attr_2) or (Attr_3 and Attr_4) or (Attr_6 and Attr_7))";
string policy_3 = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or (Attr_8 and Attr_9))";
string policy_4 = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_1, {10, 10, policy_1})->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_2, 10, 10, policy_2)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_3, 10, 10, policy_3)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_4, 10, 10, policy_4)->Unit(benchmark::kMicrosecond);

// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_4,  10, 10,  policy_4)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_20__BL_10__POLICY_4,  20, 10,  policy_4)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_30__BL_10__POLICY_4,  30, 10,  policy_4)->Unit(benchmark::kMicrosecond);

// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_100__POLICY_4,  10,  100, policy_4)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_100__BL_100__POLICY_4, 100, 100, policy_4)->Unit(benchmark::kMicrosecond);

// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Nb_WL,Nb_BL,RealTime\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        // Extract WL and BL from the benchmark name
        std::string name = run.benchmark_name();
        int nwl = extract_value(name, "WL_");
        int nbl = extract_value(name, "BL_");
        file << nwl << "," << nbl << "," << run.GetAdjustedRealTime() << "\n";
      }
    }

  private:
    std::ofstream file;

    int extract_value(const std::string& name, const std::string& prefix) {
      size_t start = name.find(prefix) + prefix.size();
      size_t end = name.find("__", start);
      return std::stoi(name.substr(start, end - start));
    }

};

int main(int argc, char** argv) {

  // InitializeOpenABE();
  if (!init_libraries()) return 1;

  for (int nwl = 0; nwl <= 100; nwl += 10) {
    for (int nbl = 0; nbl <= 100; nbl += 10) {
      std::string name = "WL_" + std::to_string(nwl) + "__BL_" + std::to_string(nbl) + "__POLICY_4";
      policy_params params = {nwl, nbl, "policy_4"};
      benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
        BM_KPABE_DPVS_SerializeDecryptionKey(state, params);
      })->Unit(benchmark::kMicrosecond);
    }
  }

  // Run benchmark
  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter("benchmark_results.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  // ShutdownOpenABE();
  clean_libraries();

  return 0;
}
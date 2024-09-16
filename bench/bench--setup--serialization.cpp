#include <benchmark/benchmark.h>
#include <string>
#include <fstream>
#include <vector>

#include "bench.hpp"

using namespace std;


// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Benchmark,RealTime,Size\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        std::string name = run.benchmark_name();
        size_t real_time = run.GetAdjustedRealTime();
        size_t size = 0;
        if (run.counters.find("Size") != run.counters.end()) {
          size = run.counters.at("Size");
        }
        file << name << "," << real_time << "," << size << "\n";
      }
    }

  private:
    std::ofstream file;
};


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
    if (!(pk_deserialized == kpabe.get_public_key())) {
      cerr << "Error: Deserialized public key is incorrect" << endl;
      exit(1);
    }
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
    if (!(mk_deserialized == kpabe.get_master_key())) {
      cerr << "Error: Deserialized master key is incorrect" << endl;
      exit(1);
    }
  }

  state.counters["Size"] = mk_bytes.size();
}

BENCHMARK(BM_KPABE_DPVS_SerializePublicKey)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_SerializeMasterKey)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_DeserializePublicKey)->Unit(benchmark::kMicrosecond);
BENCHMARK(BM_KPABE_DPVS_DeserializeMasterKey)->Unit(benchmark::kMicrosecond);

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter("benchmark--setup--serialization.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);
  // ::benchmark::RunSpecifiedBenchmarks();

  clean_libraries();

  return 0;
}

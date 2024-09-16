#include <benchmark/benchmark.h>
#include <string>
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

  OpenABEByteString dec_key_bytes;
  auto dec_key = kpabe.keygen(params.policy, wl, bl);
  dec_key->serialize(dec_key_bytes);

  // Set the custom value for size
  state.counters["Nb_WL"] = params.nwl;
  state.counters["Nb_BL"] = params.nbl;
  state.counters["Size"] = dec_key_bytes.size();
}


// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Nb_WL,Nb_BL,RealTime,Size\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        // Extract WL and BL from the benchmark name
        std::string name = run.benchmark_name();
        size_t real_time = run.GetAdjustedRealTime();
        size_t nb_wl = run.counters.at("Nb_WL");
        size_t nb_bl = run.counters.at("Nb_BL");
        size_t size = run.counters.at("Size");
        file << nb_wl << "," << nb_bl << "," << real_time << "," << size << "\n";
      }
    }

  private:
    std::ofstream file;
};


string policy = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  for (int nwl = 0; nwl <= 100; nwl += 10) {
    policy_params params = {nwl, 0, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMillisecond);
  }
  for (int nbl = 10; nbl <= 100; nbl += 10) {
    policy_params params = {0, nbl, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMillisecond);
  }
  for (int nb = 10; nb <= 100; nb += 10) {
    policy_params params = {nb, nb, policy};
    benchmark::RegisterBenchmark("BM_KPABE_DPVS_DecryptionKeyGeneration", [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMillisecond);
  }

  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter("benchmark--keygen--efficiency.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  clean_libraries();

  return 0;
}

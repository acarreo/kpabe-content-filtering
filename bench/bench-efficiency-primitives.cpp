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
// BENCHMARK(BM_KPABE_DPVS_Setup)->Unit(benchmark::kMicrosecond);

static void BM_KPABE_DPVS_DecryptionKeyGeneration(benchmark::State& state, policy_params params) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto wl = generateAttributesList("WL_url_", params.nwl);
  auto bl = generateAttributesList("BL_url_", params.nbl);

  for (auto _ : state) {
    auto dec_key = kpabe.keygen(params.policy, wl, bl);

    if (!dec_key) {
      cerr << "Error: Could not generate keys" << endl;
      exit(1);
    }
  }
}


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


string policy_4 = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  for (int nwl = 0; nwl <= 100; nwl += 10) {
    std::string name = "WL_" + std::to_string(nwl) + "__BL_" + std::to_string(0) + "__POLICY_4";
    policy_params params = {nwl, 0, "policy_4"};
    benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMicrosecond);
  }
  for (int nbl = 10; nbl <= 100; nbl += 10) {
    std::string name = "WL_" + std::to_string(0) + "__BL_" + std::to_string(nbl) + "__POLICY_4";
    policy_params params = {0, nbl, "policy_4"};
    benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMicrosecond);
  }
  for (int nb = 10; nb <= 100; nb += 10) {
    std::string name = "WL_" + std::to_string(nb) + "__BL_" + std::to_string(nb) + "__POLICY_4";
    policy_params params = {nb, nb, "policy_4"};
    benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
      BM_KPABE_DPVS_DecryptionKeyGeneration(state, params);
    })->Unit(benchmark::kMicrosecond);
  }

  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter("benchmark_efficiency-results.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  clean_libraries();

  return 0;
}
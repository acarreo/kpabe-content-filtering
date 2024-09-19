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


// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Nb_Attributes,RealTime\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        // Extract WL and BL from the benchmark name
        double real_time = run.GetAdjustedRealTime();
        size_t nb_att = run.counters.at("Nb_Attributes");
        file << nb_att << "," << std::fixed << std::setprecision(3) << real_time << "\n";
      }
    }

  private:
    std::ofstream file;
};


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
  CSVReporter csv_reporter("benchmark--encrypt--efficiency.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  clean_libraries();

  return 0;
}

#include <benchmark/benchmark.h>
#include <iomanip>
#include "bench.hpp"

using namespace std;

// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Benchmark,RealTime\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        std::string name = run.benchmark_name();
        double real_time = run.GetAdjustedRealTime();
        file << name << "," << std::fixed << std::setprecision(3) << real_time << "\n";
      }
    }

  private:
    std::ofstream file;
};

static void BM_KPABE_DPVS_Setup(benchmark::State& state) {
  for (auto _ : state) {
    KPABE_DPVS kpabe;
    if (!kpabe.setup()) {
      cerr << "Error: Could not setup KPABE_DPVS" << endl;
      exit(1);
    }
  }
}

static void BM_KPABE_DPVS_PublicKeyRandomization(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto pk = kpabe.get_public_key();
  for (auto _ : state) {
    auto __pair = pk.randomize();
  }
}

static void BM_KPABE_DPVS_ValidateDerivedPublicKey(benchmark::State& state) {
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup KPABE_DPVS" << endl;
    exit(1);
  }

  auto pk = kpabe.get_public_key();
  auto __pair = pk.randomize();
  for (auto _ : state) {
    if (!pk.validate_derived_key(__pair.first, __pair.second)) {
      cerr << "Error: Could not validate public key" << endl;
      exit(1);
    }
  }
}

BENCHMARK(BM_KPABE_DPVS_Setup)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_KPABE_DPVS_PublicKeyRandomization)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_KPABE_DPVS_ValidateDerivedPublicKey)->Unit(benchmark::kMillisecond);

int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
  // CSVReporter csv_reporter("benchmark--setup--efficiency.csv");
  // ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  clean_libraries();

  return 0;
}
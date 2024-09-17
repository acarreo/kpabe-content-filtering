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


// Custom reporter to save results to CSV
class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Nb_Attributes,RealTime,Size\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        // Extract WL and BL from the benchmark name
        size_t real_time = run.GetAdjustedRealTime();
        size_t nb_att = run.counters.at("Nb_Attributes");
        size_t size = run.counters.at("Size");
        file << nb_att << "," << real_time << "," << size << "\n";
      }
    }

  private:
    std::ofstream file;
};


int main(int argc, char** argv) {

  if (!init_libraries()) return 1;

  __relic_print_params();

  int pas = 10;
  int nb_attributes = 0;
  std::string filename = "benchmark--encrypt--";
  std::string mode;

  if (argc == 3) {
    nb_attributes = std::stoi(argv[1]);
    if (std::string(argv[2]) == "serialize") {
      mode = "serialization";
      for (int n_att = 10; n_att <= nb_attributes; n_att += pas) {
        if (n_att >= 100) pas = 100;
        benchmark::RegisterBenchmark("BM_KPABE_DPVS_SerializationCiphertext", [n_att](benchmark::State& state) {
          BM_KPABE_DPVS_SerializationCiphertext(state, n_att);
        })->Unit(benchmark::kMicrosecond);
      }
    }
    else if (std::string(argv[2]) == "deserialize") {
      mode = "deserialization";
      for (int n_att = 10; n_att <= nb_attributes; n_att += pas) {
        if (n_att >= 100) pas = 100;
        benchmark::RegisterBenchmark("BM_KPABE_DPVS_DeserializationCiphertext", [n_att](benchmark::State& state) {
          BM_KPABE_DPVS_DeserializationCiphertext(state, n_att);
        })->Unit(benchmark::kMicrosecond);
      }
    }
    else {
      std::cerr << "Usage: " << argv[0] << " <nb_attributes> <serialize|deserialize>" << std::endl;
      return 1;
    }
  }
  else {
    std::cerr << "Usage: " << argv[0] << " <nb_attributes> <serialize|deserialize>" << std::endl;
    return 1;
  }

  filename += mode + "--" + std::to_string(nb_attributes) + ".csv";

  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter(filename);
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);

  clean_libraries();

  return 0;
}

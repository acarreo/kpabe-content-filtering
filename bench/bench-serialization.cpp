#include <benchmark/benchmark.h>
#include <string>
#include <fstream>

#include "bench.hpp"

using namespace std;

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

static void BM_KPABE_DPVS_DeserializeDecryptionKey(benchmark::State& state, policy_params params) {
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

  OpenABEByteString dec_key_bytes;
  dec_key->serialize(dec_key_bytes);
  for (auto _ : state) {
    KPABE_DPVS_DECRYPTION_KEY dec_key_deserialized;
    dec_key_deserialized.deserialize(dec_key_bytes);

    // Check if deserialized decryption key is correct
    // if (!(dec_key_deserialized == *dec_key)) {
    //   cerr << "Error: Deserialized decryption key is incorrect" << endl;
    //   exit(1);
    // }
  }
}


string policy_1 = "Attr_5 and ((Attr_1 and Attr_2) or (Attr_3 and Attr_4))";
string policy_2 = "Attr_5 and ((Attr_1 and Attr_2) or (Attr_3 and Attr_4) or (Attr_6 and Attr_7))";
string policy_3 = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or (Attr_8 and Attr_9))";
string policy_4 = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";

// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_1, {10, 10, policy_1})->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_2, 10, 10, policy_2)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_3, 10, 10, policy_3)->Unit(benchmark::kMicrosecond);
// BENCHMARK_CAPTURE(BM_KPABE_DPVS_SerializeDecryptionKey, WL_10__BL_10__POLICY_4, 10, 10, policy_4)->Unit(benchmark::kMicrosecond);

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

  int __nwl = 10, __nbl = 10;
  std::string filename = "benchmark--decryption-key--serialize.csv";

  if (argc == 3) {
    __nwl = std::stoi(argv[1]);
    __nbl = __nwl;

    if (std::string(argv[2]) == "serialize") {
      for (int nwl = 0; nwl <= __nwl; nwl += 10) {
        for (int nbl = 0; nbl <= __nwl; nbl += 10) {
          std::string name = "WL_" + std::to_string(nwl) + "__BL_" + std::to_string(nbl) + "__POLICY_10";
          policy_params params = {nwl, nbl, policy_4};
          benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
            BM_KPABE_DPVS_SerializeDecryptionKey(state, params);
          })->Unit(benchmark::kMicrosecond);
        }
      }
    }
    else if (std::string(argv[2]) == "deserialize") {
      for (int nwl = 0; nwl <= __nwl; nwl += 10) {
        for (int nbl = 0; nbl <= __nwl; nbl += 10) {
          std::string name = "WL_" + std::to_string(nwl) + "__BL_" + std::to_string(nbl) + "__POLICY_10";
          policy_params params = {nwl, nbl, policy_4};
          benchmark::RegisterBenchmark(name.c_str(), [params](benchmark::State& state) {
            BM_KPABE_DPVS_DeserializeDecryptionKey(state, params);
          })->Unit(benchmark::kMicrosecond);
        }
      }
    }
    else {
      std::cerr << "Usage: " << argv[0] << " [number_attributes_WL_BL] [serialize|deserialize]" << std::endl;
      return 1;
    }
    filename = "benchmark--decryption-key--" + std::string(argv[2]) + "--" + std::to_string(__nwl) + ".csv";
  }
  else {
    std::cerr << "Usage: " << argv[0] << " [number_attributes_WL_BL] [serialize|deserialize]" << std::endl;
    return 1;
  }


  // Run benchmark
  ::benchmark::Initialize(&argc, argv);
  // CSVReporter csv_reporter(filename);
  // ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);
  ::benchmark::RunSpecifiedBenchmarks();

  // ShutdownOpenABE();
  clean_libraries();

  return 0;
}
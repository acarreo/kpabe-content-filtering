#include <benchmark/benchmark.h>
#include <string>
#include <fstream>
#include <vector>

#include "bench.hpp"

using namespace std;

typedef struct {
  std::string attributes;
  int nb_attributes;
  std::string url;
  int size;
  bool __expected;
} ciphertext_params;


class KPABEManager {
  public:
    KPABEManager() {
      if (!this->kpabe.setup()) {
        std::cerr << "Error: Could not setup KPABE_DPVS" << std::endl;
        exit(1);
      }
    }

    auto getCiphertext(std::string attributes, std::string url) {
      KPABE_DPVS_CIPHERTEXT ciphertext(attributes, url);
      uint8_t ss_key[RLC_MD_LEN];
      ciphertext.encrypt(ss_key, this->kpabe.get_public_key());

      ByteString key_bytes; key_bytes.appendArray(ss_key, RLC_MD_LEN);
      return std::make_pair(ciphertext, key_bytes);
    }

    auto getDecryptionKey(int nb_wl, int nb_bl, std::string policy) {
      auto wl = generateAttributesList("wl_url_", nb_wl);
      auto bl = generateAttributesList("bl_url_", nb_bl);

      auto dec_key = this->kpabe.keygen(policy, wl, bl);
      if (!dec_key) {
        std::cerr << "Error: Could not generate decryption key" << std::endl;
        exit(1);
      }

      return dec_key;
    }

  private:
    KPABE_DPVS kpabe;
};

static void BM_KPABE_DPVS_Decryption(benchmark::State& state, ciphertext_params params) {
  std::string policy = "(Attr_5 and (Attr_1 or Attr_2)) and ((Attr_3 and Attr_4) or (Attr_6 and Attr_7) or ((Attr_8 or Attr_9) and Attr_10))";
  if (params.nb_attributes < 10) {
    policy = "Attr_1";
  }

  KPABEManager kpabe_manager;
  auto dec_key = kpabe_manager.getDecryptionKey(params.size, params.size, policy);
  auto result = kpabe_manager.getCiphertext(params.attributes, params.url);
  auto ciphertext = result.first;

  for (auto _ : state) {
    uint8_t ss_key_rec[RLC_MD_LEN];
    auto success = ciphertext.decrypt(ss_key_rec, *dec_key);
    if (success != params.__expected) {
      std::cerr << "Error: expected " << params.__expected << " but got " << success << std::endl;
      exit(1);
    }

    ByteString ss_key_bytes; ss_key_bytes.appendArray(ss_key_rec, RLC_MD_LEN);
    if ((params.__expected == true) && (ss_key_bytes != result.second)) {
      std::cerr << "Error: session keys do not match" << std::endl;
      std::cout << ss_key_bytes.toHex() << std::endl;
      std::cout << result.second.toHex() << std::endl;
      exit(1);
    }
  }

  // Set the custom value for size
  state.counters["Nb_Attributes"] = params.nb_attributes;
  state.counters["Nb_WL_BL"] = params.size;
}


class CSVReporter : public benchmark::ConsoleReporter {
  public:
    CSVReporter(const std::string& filename) : ConsoleReporter(), file(filename) {
      file << "Benchmark,Nb_AttrCtxt,Nb_Attr_WL_BL,RealTime\n";
    }

    ~CSVReporter() {
      file.close();
    }

    void ReportRuns(const std::vector<Run>& report) override {
      for (const auto& run : report) {
        // Extract WL and BL from the benchmark name
        std::string name = run.benchmark_name();
        double real_time = run.GetAdjustedRealTime();
        size_t nb_att = run.counters.at("Nb_Attributes");
        size_t nb_wl_bl = run.counters.at("Nb_WL_BL");
        file << name << "," << nb_att << "," << nb_wl_bl << "," << real_time << "\n";
      }
    }

  private:
    std::ofstream file;
};

int main(int argc, char** argv) {
  if (!init_libraries()) return 1;

  __relic_print_params();

  std::vector<std::pair<int, int>> attrListSizes = {
    {1, 1}, {1, 10}, {1, 100}, {1, 1000},
    {10, 1}, {10, 10}, {10, 100}, {10, 1000},
    {100, 1}, {100, 10}, {100, 100}, {100, 1000},
    {1000, 1}, {1000, 10}, {1000, 100}, {1000, 1000}
  };

  for (auto num : attrListSizes)
  {
    int nb_attr_in_ciphertext = num.first;
    int taille_listes = num.second;
    std::string attributes_1 = generateAttributes(nb_attr_in_ciphertext);     // satisfies the policy
    std::string attributes_2 = generateAttributes(nb_attr_in_ciphertext, 10); // does not satisfy the policy
    std::string url_in_wl = "wl_url_1";
    std::string url_in_bl = "bl_url_1";
    std::string url = "www.example.com";

    { // attributes_1, url_in_wl
      ciphertext_params params = {attributes_1, nb_attr_in_ciphertext, url_in_wl, taille_listes, true};
      benchmark::RegisterBenchmark("Decryption_URL_in_Whitelist", [params](benchmark::State& state) {
        BM_KPABE_DPVS_Decryption(state, params);
      })->Unit(benchmark::kMillisecond);
    }

    { // attributes_1, url_in_bl
      ciphertext_params params = {attributes_1,nb_attr_in_ciphertext, url_in_bl, taille_listes, false};
      benchmark::RegisterBenchmark("Decryption_URL_in_Blacklist", [params](benchmark::State& state) {
        BM_KPABE_DPVS_Decryption(state, params);
      })->Unit(benchmark::kMillisecond);
    }

    { // attributes_1, url : satisfies the policy
      ciphertext_params params = {attributes_1, nb_attr_in_ciphertext, url, taille_listes, true};
      benchmark::RegisterBenchmark("Decryption_Policy_Satisfied", [params](benchmark::State& state) {
        BM_KPABE_DPVS_Decryption(state, params);
      })->Unit(benchmark::kMillisecond);
    }

    { // attributes_2, url : does not satisfy the policy
      ciphertext_params params = {attributes_2, nb_attr_in_ciphertext, url, taille_listes, false};
      benchmark::RegisterBenchmark("Decryption_Policy_NOT_Satisfied", [params](benchmark::State& state) {
        BM_KPABE_DPVS_Decryption(state, params);
      })->Unit(benchmark::kMillisecond);
    }
  }

  ::benchmark::Initialize(&argc, argv);
  CSVReporter csv_reporter("benchmark--decrypt--efficiency.csv");
  ::benchmark::RunSpecifiedBenchmarks(&csv_reporter);


  clean_libraries();

  return 0;
}
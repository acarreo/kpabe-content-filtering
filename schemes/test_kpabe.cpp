#include <iostream>
#include <chrono>
#include <map>

#include "kpabe.h"

using namespace std;
using namespace std::chrono;


void print_buffer(const uint8_t *buffer, size_t buffer_size) {
  for (size_t i = 0; i < buffer_size; i++) {
    printf("%02x", buffer[i]);
  }
  printf("\n");
}

vector<string> wl({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> bl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

group_setting_t params;
bn_t Fq;

int main(int argc, char **argv)
{
  if(argc < 3) {
    cout << "Usage " << string(argv[0]) << " policy attributes url" << endl;
    cout << "Example " << string(argv[0]) << " \"A5 and ((A1 and A2) or (A3 and A4))\" \"A5|A1|A2\" url" << endl;
    return -1;
  }

  string policy_str(argv[1]);
  string attributes(argv[2]);
  string url(argv[3]);

  if(!init_libraries()) return -1;

  KPABE_DPVS_master_public_key_t pk;
  KPABE_DPVS_master_secret_key_t msk;
  KPABE_DPVS_decryption_key_t sk;

  gt_t psi, xi;
  KPABE_DPVS_ciphertext_t ctx;
  // uint8_t hash1[RLC_MD_LEN];
  // uint8_t hash2[RLC_MD_LEN];

  gt_null(psi); gt_new(psi);
  gt_null(xi); gt_new(xi);

  bool result = false;

  auto start = high_resolution_clock::now();
  result = KPABE_DPVS_generate_params(pk, msk);
  auto end = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(end - start);

  if (result) {
    cout << duration.count() << "\tparams" << endl;

    start = high_resolution_clock::now();
    result = KPABE_DPVS_encrypt(ctx, psi, pk, url, attributes);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);

    if (result) {
      cout << duration.count() << "\tencrypt" << endl;

      start = high_resolution_clock::now();
      result = KPABE_DPVS_generate_decryption_key(sk, msk, policy_str, wl, bl);
      end = high_resolution_clock::now();
      duration = duration_cast<microseconds>(end - start);

      if (result) {
        cout << duration.count() << "\tdec_key" << endl;

        start = high_resolution_clock::now();
        result = KPABE_DPVS_decrypt(xi, ctx, sk, url);
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);

        cout << duration.count() << "\tdecrypt" << endl;

        // if (gt_cmp(psi, xi) == RLC_EQ) cout << "====>\tDecryption SUCCESS" << endl;
        // else cout << "---->\tDecryption FAIL" << endl;
      }
    }
  }


  KPABE_DPVS_decryption_key_destroy(sk);
  KPABE_DPVS_master_secret_key_destroy(msk);
  KPABE_DPVS_master_public_key_destroy(pk);
  KPABE_DPVS_ciphertext_destroy(ctx);
  gt_free(psi);
  gt_free(xi);

  clean_libraries();

  return 0;
}

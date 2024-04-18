#include <iostream>
#include <fstream>
#include <chrono>

#include "kpabe/kpabe.hpp"
#include "keys/keys.hpp"

using namespace std;


vector<string> white_list;
vector<string> black_list;

vector<string> generateAttributesList(string prefix, int n) {
  vector<string> list;
  for (int i = 0; i < n; i++) {
    list.push_back(prefix + to_string(i));
  }
  return list;
}

vector<string> wl({"A:www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> bl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

const vector<string> list_policies({
  "(A_05 and (A_01 and A_02))",
  "(A_00 and A_01) or (A_03 and A_04)",
  "A_07 and (A_06 or A_05) and (A_04 or A_03)",
  "A_05 and (A_01 and A_02) or (A_03 and A_04)",
  "(A_07 or A_08) and (A_09 or A_06) and (A_01 or A_02)",
});


void test_serialization_keys();
void test_encryption();

using namespace std::chrono;

int main(int argc, char **argv) {

  if (!init_libraries()) return 2;

  white_list = generateAttributesList("WL_url_", 10);
  black_list = generateAttributesList("BL_url_", 10);

  test_serialization_keys();

  test_encryption();


#if 0
  // if(argc < 3) {
  //   cout << "Usage " << string(argv[0]) << " policy attributes url" << endl;
  //   cout << "Example " << string(argv[0]) << " \"A5 and ((A1 and A2) or (A3 and A4))\" \"A5|A1|A2\" url" << endl;
  //   return 1;
  // }

  // string policy(argv[1]);
  // string attributes(argv[2]);
  // string url(argv[3]);
#endif

  clean_libraries();

  return 0;
}


void test_serialization_keys() {
  KPABE_DPVS kpabe(wl, bl);

  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();
  auto dec_key = kpabe.keygen("A5 and ((A1 and A2) or (A3 and A4))");

  KPABE_DPVS_PUBLIC_KEY public_key2;
  KPABE_DPVS_MASTER_KEY master_key2;


  // Test serialization of public and master keys

  ByteString public_key_bytes, master_key_bytes;

  public_key.serialize(public_key_bytes);
  master_key.serialize(master_key_bytes);

  public_key2.deserialize(public_key_bytes);
  master_key2.deserialize(master_key_bytes);


  if (public_key == public_key2 && master_key == master_key2) {
    std::cout << "Public and master keys are equal" << std::endl;
  } else {
    std::cerr << "Error: Public and master keys are not equal" << std::endl;
  }

  // cout << "Time ----------> " << duration_cast<microseconds>(stop - start).count() << " us" << endl;
  // Test serialization of decryption key

  KPABE_DPVS_DECRYPTION_KEY dec_key2;

  ByteString dec_key_bytes;
  dec_key->serialize(dec_key_bytes);
  dec_key2.deserialize(dec_key_bytes);

  if (*dec_key == dec_key2) {
    std::cout << "Decryption keys are equal" << std::endl;
  } else {
    std::cerr << "Error: Decryption keys are not equal" << std::endl;
  }

  cout << "\nSizeof PK function ------> " << public_key.getSizeInBytes() << endl;
  cout << "Public key size ---------> " << public_key_bytes.size() << endl;
  cout << "\nSizeof MK function ------> " << master_key.getSizeInBytes() << endl;
  cout << "Master key size ---------> " << master_key_bytes.size() << endl;
  cout << "\nSizeof DK function ------> " << dec_key->getSizeInBytes() << endl;
  cout << "Decryption key size -----> " << dec_key_bytes.size() << endl;
}

void test_encryption() {
  KPABE_DPVS kpabe(white_list,bl);// black_list);

  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto pk = kpabe.get_public_key();
  auto mk = kpabe.get_master_key();

  auto dec_key = kpabe.keygen("A5 and ((A1 and A2) or (A3 and A4))");

  // Test encryption
  string url("www.facebook.com");
  string attributes("|A5|A1|A2|AA|BB|CC|");

  uint8_t key[RLC_MD_LEN];
  uint8_t key_rec[RLC_MD_LEN];

  KPABE_DPVS_CIPHERTEXT cipher(attributes, url);

  auto start_enc = high_resolution_clock::now();
  bool success = cipher.encrypt(key, pk);
  auto stop_enc = high_resolution_clock::now();

  cout << "Time Enc ----------> " << duration_cast<microseconds>(stop_enc - start_enc).count() << " us" << endl;
  if (!success) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return;
  }

  ByteString cipher_bytes;
  cipher.serialize(cipher_bytes);

  cout << "\nSizeof CT function ------> " << cipher.getSizeInBytes() << endl;
  cout << "Ciphertext size ---------> " << cipher_bytes.size() << endl;


  auto start_dec = high_resolution_clock::now();
  success = cipher.decrypt(key_rec, *dec_key);
  auto stop_dec = high_resolution_clock::now();

  cout << "Time Dec ----------> " << duration_cast<microseconds>(stop_dec - start_dec).count() << " us" << endl;
  if (!success) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }


  if (memcmp(key, key_rec, RLC_MD_LEN) == 0) {
    std::cout << "Session keys are equal" << std::endl;
  } else {
    std::cerr << "Error: Session keys are not equal" << std::endl;
  }
}

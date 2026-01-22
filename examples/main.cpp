#include <iostream>
#include <fstream>
#include <chrono>

#include "kpabe.hpp"
#include "keys.hpp"

using namespace std;
using namespace std::chrono;


void test_serialization_keys();
void test_encryption();


const vector<string> black_list({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
const vector<string> white_list({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

int main(int argc, char **argv) {

  cout << "\n--------> Attribute-Based Encryption (ABE) scheme for Web Content Filtering - Test Application <--------" << endl;

  cout << "\n--------> Initializing cryptographic libraries" << endl;
  if (!init_libraries()) {
    std::cerr << "Error: Could not initialize cryptographic libraries" << std::endl;
    return 1;
  }

  string attributs="pegi3|no-in-game-purchase|no-gambling|no-violence|no-discrimination|no-horror|no-bad-language|no-sex|no-drugs";
  string policy_1 ="(pegi3 and no-in-game-purchase) or (pegi16 and no-in-game-purchase)";
  string policy_2 ="(pegi3 and no-in-game-purchase) or (pegi16 and no-sex)";

  string url="www.google.com";

  KPABE_DPVS kpabe;

  cout << "\n--------> Generating KP-ABE params : master public and secret keys" << endl;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return 2;
  }

  cout << "\n--------> Generating Decryption Key for Policy 1: " << policy_1 << endl;
  auto dk1 = kpabe.keygen(policy_1, white_list, black_list);
  if (!dk1) {
    std::cerr << "Error: Could not generate decryption key for the Policy 1" << std::endl;
    return 3;
  }
  cout << "========> Size of Decryption Key 1: " << dk1->getSizeInBytes() << " bytes" << endl;

  cout << "\n--------> Generating Decryption Key for Policy 2: " << policy_2 << endl;
  auto dk2 = kpabe.keygen(policy_2, white_list, black_list);
  if (!dk2) {
    std::cerr << "Error: Could not generate decryption key for the Policy 2" << std::endl;
    return 4;
  }
  cout << "========> Size of Decryption Key 2: " << dk2->getSizeInBytes() << " bytes" << endl;

  uint8_t key[RLC_MD_LEN];
  uint8_t key1_rec[RLC_MD_LEN];
  uint8_t key2_rec[RLC_MD_LEN];


  cout << "\n--------> Request ressource '" << url << "' associated with attributes:" << endl;
  cout << attributs << endl;
  KPABE_DPVS_CIPHERTEXT cipher(attributs, url);

  auto pk = kpabe.get_public_key();
  if (!cipher.encrypt(key, pk)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return 5;
  }
  cout << "\nKP-ABE Encryption successful and the requested ressource is encrypted under a session key.";
  cout << "\n========> Session key:\n          ";
  for (size_t i = 0; i < RLC_MD_LEN; i++) printf("%02X", key[i]);
  cout << "\n========> ABE Ciphertext size: " << cipher.getSizeInBytes() << " bytes" << endl;
  
  cout << "\n--------> Decrypting ABE ciphertext with 'Decryption Key 1':" << endl;
  if (!cipher.decrypt(key1_rec, *dk1)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
  } else {
    cout << "          ";
    for (size_t i = 0; i < RLC_MD_LEN; i++) printf("%02X", key1_rec[i]);
    cout << endl;
    if (memcmp(key, key1_rec, RLC_MD_LEN) == 0) {
      std::cout << "========> Decrypted session key is equal to original session key" << std::endl;
    } else {
      std::cerr << "Error: Session keys are not equal for Policy 1" << std::endl;
    }
  }

  cout << "\n--------> Decrypting ABE ciphertext with 'Decryption Key 2':" << endl;
  if (!cipher.decrypt(key2_rec, *dk2)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
  } else {
    cout << "          ";
    for (size_t i = 0; i < RLC_MD_LEN; i++) printf("%02X", key2_rec[i]);
    cout << endl;
    if (memcmp(key, key2_rec, RLC_MD_LEN) == 0) {
      std::cout << "========> Decrypted session key is equal to original session key" << std::endl;
    } else {
      std::cerr << "Error: Session keys are not equal for Policy 2" << std::endl;
    }
  }

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

  cout << "\n--------> Cleaning up cryptographic libraries\n" << endl;
  clean_libraries();

  return 0;
}


void test_serialization_keys() {
  KPABE_DPVS kpabe;

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

  cout << "----------------------------------------------------------------------" << endl;
  cout << public_key_bytes.toHex() << endl;
  cout << "----------------------------------------------------------------------" << endl;

  cout << "\nSizeof PK function ------> " << public_key.getSizeInBytes() << endl;
  cout << "Public key size ---------> " << public_key_bytes.size() << endl;
  cout << "\nSizeof MK function ------> " << master_key.getSizeInBytes() << endl;
  cout << "Master key size ---------> " << master_key_bytes.size() << endl;
  cout << "\nSizeof DK function ------> " << dec_key->getSizeInBytes() << endl;
  cout << "Decryption key size -----> " << dec_key_bytes.size() << endl;
}

void test_encryption() {
  KPABE_DPVS kpabe(white_list, black_list);

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

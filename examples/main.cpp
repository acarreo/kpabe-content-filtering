#include <iostream>
#include <fstream>
#include <chrono>

#include "kpabe.hpp"
#include "keys.hpp"

using namespace std;
using namespace std::chrono;


const vector<string> black_list({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
const vector<string> white_list({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

int main(int argc, char **argv) {

  cout << "\n--------> Attribute-Based Encryption (ABE) scheme for Web Content Filtering - Test Application <--------" << endl;

  cout << "\n--------> Initializing cryptographic libraries" << endl;
  if (!init_libraries()) {
    std::cerr << "Error: Could not initialize cryptographic libraries" << std::endl;
    return 1;
  }

  string attributes="pegi3|no-in-game-purchase|no-gambling|no-violence|no-discrimination|no-horror|no-bad-language|no-sex|no-drugs";
  string policy="(pegi3 and no-in-game-purchase) or (pegi16 and no-in-game-purchase)";
  string url="www.some-ressources.com";

  if (argc == 4) {
    policy     = argv[1];
    attributes = argv[2];
    url        = argv[3];
  } else {
    cout << "\n--------> Using default values of policy, attributes and requested url --------" << endl;
  }

  cout << "\n--------> Generating KP-ABE params : master public and secret keys" << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return 2;
  }

  cout << "\n--------> Generating Decryption Key for the policy : " << policy << endl;
  auto dk = kpabe.keygen(policy, white_list, black_list);
  if (!dk) {
    std::cerr << "Error: Could not generate decryption key for the Policy" << std::endl;
    return 3;
  }
  cout << "========> Size of Decryption Key: " << dk->getSizeInBytes() << " bytes" << endl;

  uint8_t key[RLC_MD_LEN];
  uint8_t key_rec[RLC_MD_LEN];

  cout << "\n--------> Request ressource '" << url << "' associated with attributes:" << endl;
  cout << attributes << endl;
  KPABE_DPVS_CIPHERTEXT cipher(attributes, url);

  auto pk = kpabe.get_public_key();
  if (!cipher.encrypt(key, pk)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return 5;
  }
  cout << "\nKP-ABE Encryption successful and the requested ressource is encrypted under a session key.";
  cout << "\n========> Session key:\n          ";
  for (size_t i = 0; i < RLC_MD_LEN; i++) printf("%02X", key[i]);
  cout << "\n========> ABE Ciphertext size: " << cipher.getSizeInBytes() << " bytes" << endl;
  
  cout << "\n--------> Decrypting ABE ciphertext with 'Decryption Key':" << endl;
  if (!cipher.decrypt(key_rec, *dk)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
  } else {
    cout << "          ";
    for (size_t i = 0; i < RLC_MD_LEN; i++) printf("%02X", key_rec[i]);
    cout << endl;
    if (memcmp(key, key_rec, RLC_MD_LEN) == 0) {
      std::cout << "========> Decrypted session key is equal to original session key" << std::endl;
    } else {
      std::cerr << "Error: Session keys are not equal" << std::endl;
    }
  }

  cout << "\n--------> Cleaning up cryptographic libraries\n" << endl;
  clean_libraries();

  return 0;
}


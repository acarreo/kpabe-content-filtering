#include <iostream>
#include <fstream>

#include "kpabe/kpabe.hpp"

using namespace std;

bn_t Fq;

vector<string> wl({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> bl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});


int main(int argc, char **argv) {

  if (!init_libraries()) return 2;

  if(argc < 3) {
    cout << "Usage " << string(argv[0]) << " policy attributes url" << endl;
    cout << "Example " << string(argv[0]) << " \"A5 and ((A1 and A2) or (A3 and A4))\" \"A5|A1|A2\" url" << endl;
    return 1;
  }

  string policy(argv[1]);
  string attributes(argv[2]);
  string url(argv[3]);

  // Create the KP-ABE scheme and setup it
  KPABE_DPVS kpabe(wl, bl);
  kpabe.setup();

  auto public_key = kpabe.get_public_key();
  auto dec_key = kpabe.keygen(policy);
  
  if (!dec_key) {
    cout << "Error during the generation of the decryption key" << endl;
    return 1;
  }

  bn_t psi;
  uint8_t session_key[32];
  uint8_t session_key2[32];

  // Derive session key and encrypt
  bn_null(psi); bn_new(psi); bn_rand_mod(psi, Fq);
  derive_session_key(session_key, psi);

  KPABE_DPVS_CIPHERTEXT ciphertext(attributes, url);
  ciphertext.encrypt(psi, public_key);

  // Decryption
  if (ciphertext.decrypt(session_key2, *dec_key) &&
      memcmp(session_key, session_key2, 32) == 0)
  {
    cout << "Decryption Successful: the session key is recovered" << endl;
  } else {
    cout << "Decryption Failed" << endl;
  }

  bn_free(psi);
  clean_libraries();

  return 0;
}

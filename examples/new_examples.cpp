/**
 * @file new_examples.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief New examples for the KP-ABE scheme using DPVS
 * @date 2024-03-23
 *
 */

#include <iostream>
#include <fstream>

#include "kpabe.hpp"

using namespace std;

vector<string> wl({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> bl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

string policy("A5 and ((A1 and A2) or (A3 and A4))");

uint8_t session_key_00[RLC_MD_LEN]; // RLC_MD_LEN = 32 bytes ~ 256 bits
uint8_t session_key_01[RLC_MD_LEN];
uint8_t session_key_02[RLC_MD_LEN];


void example_byteString() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  ByteString bytes;
  stringstream ss;

  bytes.fillBuffer(0, 32);
  rand_bytes(bytes.getInternalPtr(), bytes.size());

  // byteString to stringstream
  ss << bytes;

  // byteString from stringstream
  ByteString bytes_from_ss;
  bytes_from_ss.fromString(ss.str());

  cout << "ByteString :" << endl;
  cout << bytes.toHex() << endl;
  cout << "ByteString from stringstream :" << endl;
  cout << bytes_from_ss.toHex() << endl;
}

void example_generate_params() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Generating public and master keys..." << std::endl;

  // KPABE_DPVS kpabe_old(wl, bl); // old version
  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();
}

void example_generate_decryption_key() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Generating decryption key..." << std::endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  // auto dec_key_old = kpabe_old.keygen(policy); // old version
  auto dec_key_1 = kpabe.keygen(policy, wl, bl);       // No hash attributes
  auto dec_key_2 = kpabe.keygen(policy, wl, bl, true); // Hash attributes


  // Decryption key from master key
  auto master_key = kpabe.get_master_key();

  KPABE_DPVS_DECRYPTION_KEY dec_key_3(policy, wl, bl); // No hash attributes (old version)
  if (!dec_key_3.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key -- No hash attributes" << std::endl;
    return;
  }

  KPABE_DPVS_DECRYPTION_KEY dec_key_4(policy, wl, bl, true); // Hash attributes
  if (!dec_key_4.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key -- Hash attributes" << std::endl;
    return;
  }
}

void example_encrypt_decrypt_old() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Encrypting and decrypting -- Old version..." << std::endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();

  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl);
  if (!dec_key.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key" << std::endl;
    return;
  }

  uint8_t sym_key[RLC_MD_LEN];
  uint8_t sym_key_rec[RLC_MD_LEN];

  string url("www.perdu.com");
  string attributes("|A5|A1|A2|AA|BB|CC|");

  /******************************* ENCRYPTION *******************************/
  KPABE_DPVS_CIPHERTEXT cipher(attributes, url);
  if (!cipher.encrypt(sym_key, public_key)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return;
  }

  // old version : NOT WORKING
  // bn_t phi;
  // generate_session_key(sym_key, phi);
  // ciphert.encrypt(phi, public_key);


  /******************************* DECRYPTION *******************************/
  if (!cipher.decrypt(sym_key_rec, dec_key)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  if (memcmp(sym_key, sym_key_rec, RLC_MD_LEN) == 0) {
    std::cout << "Symmetric key recovered successfully" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key NOT recovered" << std::endl;
  }
}

/* The only difference between this function and the previous one is the
 * hash_attr parameter. This parameter is set to true in this function.
 */
void example_encrypt_decrypt() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Encrypting and decrypting..." << std::endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();

  uint8_t sym_key[RLC_MD_LEN];
  uint8_t sym_key_rec[RLC_MD_LEN];

  string url("www.perdu.com");
  string attributes("|A5|A1|A2|AA|BB|CC|");


  /******************************* ENCRYPTION *******************************/
  KPABE_DPVS_CIPHERTEXT cipher(attributes, url, true); // Hash attributes
  if (!cipher.encrypt(sym_key, public_key)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return;
  }

  /***************************** DECRYPTION KEYS *****************************/
  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl, true); // Hash attributes
  if (!dec_key.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key" << std::endl;
    return;
  }

  /******************************* DECRYPTION *******************************/
  if (!cipher.decrypt(sym_key_rec, dec_key)) {
    std::cerr << "Error: Could not decrypt -- Hash attributes" << std::endl;
    return;
  }
  if (memcmp(sym_key, sym_key_rec, RLC_MD_LEN) == 0) {
    std::cout << "Symmetric key recovered successfully -- Hash attributes" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key NOT recovered -- Hash attributes" << std::endl;
  }
}


void example_unlinkability_public_keys() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Testing unlinkability of public keys..." << std::endl;

  uint8_t sym_key[RLC_MD_LEN];
  uint8_t sym_key_rec_1[RLC_MD_LEN];
  uint8_t sym_key_rec_2[RLC_MD_LEN];

  string url("www.perdu.com");
  string attributes("|A5|A1|A2|AA|BB|CC|");

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();

  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl);
  if (!dec_key.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key" << std::endl;
    return;
  }

  // std::pair<KPABE_DPVS_PUBLIC_KEY, ZP> pk_rand = public_key.randomize();
  // KPABE_DPVS_PUBLIC_KEY public_key_rand = pk_rand.first;
  // ZP zp = pk_rand.second;
  auto pk_rand = public_key.randomize();
  auto public_key_rand = pk_rand.first;
  auto rand = pk_rand.second; // corresponds to bn_t rand in old version

  // old version : NOT WORKING
  // bn_t rand;
  // auto pk_unlink = public_key.randomize(rand);


  /*************************** Unlinkability *****************************/

  BPGroup group;
  ZP random_value;
  random_value.setRandom(group.order);

  if (public_key.validate_derived_key(public_key_rand, rand) &&
      !public_key.validate_derived_key(public_key_rand, random_value)) {
    std::cout << "The randomized public key is valid" << std::endl;
  } else {
    std::cerr << "Error: The randomized public key is NOT valid" << std::endl;
    return;
  }

  /******************************* ENCRYPTION *******************************/
  std::cout << "Encrypting and decrypting with randomized public key..." << std::endl;

  KPABE_DPVS_CIPHERTEXT cipher(attributes, url);
  if (!cipher.encrypt(sym_key, public_key_rand)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return;
  }

  /******************************* DECRYPTION *******************************/
  std::cout << "\nDecrypting WITHOUT REMOVING the scalar from the ciphertext..." << std::endl;
  if (!cipher.decrypt(sym_key_rec_1, dec_key)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  // The decryption still success but the symmetric key is not recovered
  if (memcmp(sym_key, sym_key_rec_1, RLC_MD_LEN) != 0) {
    std::cout << "Symmetric key NOT recovered" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key recovered" << std::endl;
    return;
  }

  std::cout << "\nDecrypting with provided the randomizer..." << std::endl;
  if (!cipher.decrypt(sym_key_rec_2, dec_key, rand)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  if (memcmp(sym_key, sym_key_rec_2, RLC_MD_LEN) == 0) {
    std::cout << "Symmetric key recovered successfully" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key NOT recovered" << std::endl;
    return;
  }

  std::cout << "\nDecrypting AFTER REMOVING the scalar from the ciphertext..." << std::endl;
  cipher.remove_scalar(rand); // Remove the randomization from the ciphertext, before decrypting
  if (!cipher.decrypt(sym_key_rec_2, dec_key)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  if (memcmp(sym_key, sym_key_rec_2, RLC_MD_LEN) == 0) {
    std::cout << "Symmetric key recovered successfully" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key NOT recovered" << std::endl;
    return;
  }
}


// Old serialization methods can be found on examples/examples.cpp
// Other serialization examples can be found on main.cpp : test_serialization_keys()
void example_serialization() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  std::cout << "Testing serialization..." << std::endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();

  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl);
  if (!dec_key.generate(master_key)) {
    std::cerr << "Error: Could not generate decryption key" << std::endl;
    return;
  }

  uint8_t sym_key[RLC_MD_LEN];
  uint8_t sym_key_rec[RLC_MD_LEN];
  uint8_t sym_key_rec_1[RLC_MD_LEN];

  string url("www.perdu.com");
  string attributes("|A5|A1|A2|AA|BB|CC|");

  /******************************* ENCRYPTION *******************************/
  KPABE_DPVS_CIPHERTEXT cipher(attributes, url);
  if (!cipher.encrypt(sym_key, public_key)) {
    std::cerr << "Error: Could not encrypt" << std::endl;
    return;
  }

  size_t sizeof_cipher = cipher.getSizeInBytes();
  char* buffer_cipher = (char*) malloc(sizeof_cipher * sizeof(char));
  if (buffer_cipher == NULL) {
    std::cerr << "Error: Could not allocate memory" << std::endl;
    return;
  }
  std::cout << "\nSize of Ciphertext : getSizeInBytes() --> " << sizeof_cipher << std::endl;


  // Serialize the ciphertext
  vector<uint8_t> cipher_bytes;
  cipher.serialize(cipher_bytes);

  // Copy the serialized ciphertext to the buffer
  if (sizeof_cipher != cipher_bytes.size()) {
    std::cerr << "Error: Size of Ciphertext mismatch : " << cipher_bytes.size() << std::endl;
    return;
  }
  memcpy(buffer_cipher, cipher_bytes.data(), sizeof_cipher);

  // Deserialize the ciphertext from the buffer
  KPABE_DPVS_CIPHERTEXT cipher_deserialized;
  vector<uint8_t> bytes_from_buffer(buffer_cipher, buffer_cipher + sizeof_cipher);
  cipher_deserialized.deserialize(bytes_from_buffer);

  /******************************* DECRYPTION *******************************/
  std::cout << "\nDecrypting the deserialized ciphertext..." << std::endl;
  if (!cipher_deserialized.decrypt(sym_key_rec, dec_key)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  if (memcmp(sym_key, sym_key_rec, RLC_MD_LEN) == 0) {
    std::cout << "Symmetric key recovered successfully" << std::endl;
  } else {
    std::cerr << "Error: Symmetric key NOT recovered" << std::endl;
    return;
  }

  // The ==operator is not implemented for the ciphertext
  // In place, we can compare the decrypted symmetric keys from the two ciphertexts
  std::cout << "\nDecrypting the original ciphertext..." << std::endl;
  if (!cipher.decrypt(sym_key_rec_1, dec_key)) {
    std::cerr << "Error: Could not decrypt" << std::endl;
    return;
  }
  if (memcmp(sym_key_rec, sym_key_rec_1, RLC_MD_LEN) == 0) {
    std::cout << "The serialized and deserialized ciphertexts are equal" << std::endl;
  } else {
    std::cerr << "Error: Serialized and deserialized ciphertexts are NOT equal" << std::endl;
  }
}

void example_serialization_zp () {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  BPGroup group;
  ZP zp;
  zp.setRandom(group.order);

  ByteString bytes;
  zp.serialize(bytes);

  unsigned char* zp_char = bytes.data();
  size_t len = bytes.size();

  ZP zp_deserialized;
  ByteString raw_data;
  raw_data.assign(zp_char, zp_char + len);
  zp_deserialized.deserialize(raw_data);

  if (zp == zp_deserialized) {
    cout << "ZP serialization/deserialization successful" << endl;
  } else {
    cerr << "ZP serialization/deserialization failed" << endl;
  }
}

void example_serialization_randomized_public_key() {
  std::cout << "\n----------------> START : " << __func__ << std::endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    std::cerr << "Error: Could not setup keys" << std::endl;
    return;
  }

  auto public_key = kpabe.get_public_key();

  // Randomize the public key
  auto pk_rand = public_key.randomize();

  // Serialization
  vector<uint8_t> pk_bytes;
  ByteString zp_bytes;

  pk_rand.first.serialize(pk_bytes);
  pk_rand.second.serialize(zp_bytes);

  // Deserialization
  KPABE_DPVS_PUBLIC_KEY pk_deserialized;
  ZP zp_deserialized;

  pk_deserialized.deserialize(pk_bytes);
  zp_deserialized.deserialize(zp_bytes);

  // Validate the derived key, which should be valid
  if (public_key.validate_derived_key(pk_deserialized, zp_deserialized)) {
    std::cout << "The deserialized randomized public key is valid" << std::endl;
  } else {
    std::cerr << "Error: The deserialized randomized public key is NOT valid" << std::endl;
  }
}


int main()
{
  if (!init_libraries()) return 2;

  example_generate_params();
  example_generate_decryption_key();
  example_encrypt_decrypt_old();
  example_encrypt_decrypt();
  example_unlinkability_public_keys();
  example_serialization();
  example_serialization_zp();
  example_serialization_randomized_public_key();

  example_byteString();

  clean_libraries();
  return 0;
}


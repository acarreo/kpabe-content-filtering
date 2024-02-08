/**
 * @file examples.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Examples for the KP-ABE scheme using DPVS
 * @date 2023-08-20
 *
 */

#include "examples.hpp"
#include <iomanip>

using namespace std;

vector<string> wl({"www.facebook.com", "www.twitter.com", "www.instagram.com", "www.youtube.com", "www.tiktok.com"});
vector<string> bl({"www.example.com", "www.google.com", "www.amazon.com", "www.nytimes.com", "www.wikipedia.org"});

string policy_00("A5 and ((A1 and A2) or (A3 and A4))");
string policy_01("(A1 and A2) or (A3 and A4)");
string policy_02("A5 and (A1 and A2)");

string public_key_file("public_key.bin");
string master_key_file("master_key.bin");

string ciphertext_file_00("ciphertext_00.bin");
string ciphertext_file_01("ciphertext_01.bin");
string ciphertext_file_02("ciphertext_02.bin");

string dec_key_file_00("dec_key_00.bin");
string dec_key_file_01("dec_key_01.bin");
string dec_key_file_02("dec_key_02.bin");

uint8_t session_key_00[RLC_MD_LEN]; // RLC_MD_LEN = 32 bytes ~ 256 bits
uint8_t session_key_01[RLC_MD_LEN];
uint8_t session_key_02[RLC_MD_LEN];

/**
 * @brief Generate public and private params and export them
 * 
 */
void generate_params() {

  // Create the KP-ABE scheme and setup it
  KPABE_DPVS kpabe(wl, bl);
  kpabe.setup();

  kpabe.export_public_key(public_key_file);
  kpabe.export_master_key(master_key_file);

  // Generate and export the decryption key. Here we show that we can
  // generate a decryption key for a user from kpabe object.
  kpabe.keygen(policy_00)->saveToFile(dec_key_file_00);
}


/**
 * @brief For a given policy, generate and export a decryption key.
 *
 * @param[in] policy Policy associated to the key
 * @param[in] master_key_file master secret key file
 * @param[out] dec_key_file The decryption key is exported to this file
 */
void generate_decryption_key(string policy, string master_key_file, string dec_key_file) {

  // Load the master key
  KPABE_DPVS_MASTER_KEY master_key(master_key_file);

  // Set policy and generate the decryption key
  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, wl, bl);
  if (dec_key.generate(master_key)) {
    dec_key.saveToFile(dec_key_file);
  }
  else {
    cerr << "Error during the generation of the decryption key" << endl;
  }
}

/**
 * @brief Generate a session key and export the ciphertext
 *
 * @param[out] session_key must be a pointer to a buffer of size RLC_MD_LEN (32 bytes)
 * @param[out] ciphertext_file The ciphertext is exported to this file
 * @param[in] url The requested URL
 * @param[in] attributes The attributes associated to the url
 * @param[in] public_key_file The public key file
 */
void encrypt_and_export(uint8_t* session_key, string url, string attributes,
                        string public_key_file, string ciphertext_file) {

  // Load the public key
  KPABE_DPVS_PUBLIC_KEY public_key(public_key_file);

  // Generate a session key
  bn_t phi;
  generate_session_key(session_key, phi);

  /* Set the attributes and url, to the KPABE_DPVS_CIPHERTEXT object, and
   * then encrypt the random seed `phi` */
  KPABE_DPVS_CIPHERTEXT ciphertext(attributes, url);
  bool is_encrypt = ciphertext.encrypt(phi, public_key);
  if (is_encrypt) {
    ciphertext.saveToFile(ciphertext_file);
  }
  else {
    cout << "Encryption failed" << endl;
  }

  bn_free(rand);
}

/**
 * @brief Decrypt the ciphertext and export the session key
 *
 * @param[out] session_key must be a pointer to a buffer of size RLC_MD_LEN (32 bytes)
 * @param[in] ciphertext_file The ciphertext file
 * @param[in] dec_key_file The decryption key file
 */
void decrypt(uint8_t* session_key, string ciphertext_file, string dec_key_file) {

  // Load the ciphertext
  KPABE_DPVS_CIPHERTEXT ciphertext(ciphertext_file);

  // Load the decryption key
  KPABE_DPVS_DECRYPTION_KEY dec_key(dec_key_file);

  // Decryption
  bool is_decrypt = ciphertext.decrypt(session_key, dec_key);
  if (is_decrypt) {
    cout << "Decryption success" << endl;
  }
  else {
    cout << "Decryption failed" << endl;
  }
}


void example_generate_keys() {

  // Generate public and private params and export theme. A decryption key for
  // the policy_00 is generated and exported to the file dec_key_file_00.
  generate_params();

  // Generate a decryption key for the policy_01 and policy_02, and export them
  generate_decryption_key(policy_01, master_key_file, dec_key_file_01);
  generate_decryption_key(policy_02, master_key_file, dec_key_file_02);
}

void example_encryption() {
  
  // URL in white list
  encrypt_and_export(session_key_00, wl[1], "A1|A2|A5",
                     public_key_file, ciphertext_file_00);

  // URL in black list
  encrypt_and_export(session_key_01, bl[3], "A1|A2|A3|A4|A5",
                     public_key_file, ciphertext_file_01);

  // URL is not in white list nor in black list.
  // The set of attributes satisfies the policy_00 and the policy_01
  encrypt_and_export(session_key_02, "www.url.com", "A3|A4|A5",
                     public_key_file, ciphertext_file_02);

}

void example_decryption() {

  uint8_t sess_key_00[RLC_MD_LEN];
  uint8_t sess_key_01[RLC_MD_LEN];
  uint8_t sess_key_02[RLC_MD_LEN];

  decrypt(sess_key_00, ciphertext_file_00, dec_key_file_00); // SUCCESS (expected)
  // Check if the session keys are equal
  if (memcmp(sess_key_00, session_key_00, RLC_MD_LEN) == 0) {
    cout << "Session key 00: RECOVERED" << endl;
  }
  else {
    cout << "Session key 00: FAILED" << endl;
  }
  cout << endl;

  decrypt(sess_key_01, ciphertext_file_01, dec_key_file_01); // UNRECOVER (expected)
  // Check if the session keys are equal
  if (memcmp(sess_key_01, session_key_01, RLC_MD_LEN) != 0) {
    cout << "Session key 01: NOT RECOVERED" << endl;
  }
  else {
    cout << "Session key 01: FAILED" << endl;
  }
  cout << endl;

  decrypt(sess_key_02, ciphertext_file_02, dec_key_file_01); // SUCCESS (expected)
  // Check if the session keys are equal
  if (memcmp(sess_key_02, session_key_02, RLC_MD_LEN) == 0) {
    cout << "Session key 02: RECOVER" << endl;
  }
  else {
    cout << "Session key 02: FAILED" << endl;
  }
  cout << endl;

  // Attempt to decrypt ciphertext_02 using dec_key_02
  // The decryption is expected to fail as the policy associated with dec_key_02
  // is not satisfied by the set of attributes linked to ciphertext_02
  uint8_t sess_key_02_02[RLC_MD_LEN];
  decrypt(sess_key_02_02, ciphertext_file_02, dec_key_file_02); // UNRECOVER (expected)
  if (memcmp(sess_key_02_02, session_key_02, RLC_MD_LEN) != 0) {
    cout << "Session key 02: NOT RECOVERED" << endl;
  }
  else {
    cout << "Session key 02: FAILED" << endl;
  }
  cout << endl;
}

void example_serialization() {

  // Encryption and decryption
  uint8_t sess_key_0[RLC_MD_LEN];
  uint8_t sess_key_1[RLC_MD_LEN];
  uint8_t sess_key_2[RLC_MD_LEN];

  bn_t phi;

  // Create the KP-ABE scheme and setup it
  KPABE_DPVS kpabe(wl, bl);
  kpabe.setup();

  // Generate a decryption key from kpabe object
  auto dec_key_2 = kpabe.keygen(policy_00);


  /**************************************************************************/

  // Export public and master keys to buffer
  vector<uint8_t> buffer_public_key, buffer_master_key;
  kpabe.export_public_key(buffer_public_key);
  kpabe.export_master_key(buffer_master_key);

  // Load public and master keys from buffer
  KPABE_DPVS_PUBLIC_KEY public_key;
  KPABE_DPVS_MASTER_KEY master_key;
  public_key.deserialize(buffer_public_key);
  master_key.deserialize(buffer_master_key);

  // Generate a decryption key for the policy_00, using the master key from buffer
  KPABE_DPVS_DECRYPTION_KEY dec_key_1(policy_00, wl, bl);
  dec_key_1.generate(master_key);

  KPABE_DPVS_CIPHERTEXT ciphertext("A1|A2|A5", "url");
  generate_session_key(sess_key_0, phi);
  ciphertext.encrypt(phi, public_key);
  ciphertext.decrypt(sess_key_1, dec_key_1);

  std::vector<uint8_t> buffer_ciphertext;
  ciphertext.serialize(buffer_ciphertext);

  KPABE_DPVS_CIPHERTEXT ciphertext_2;
  ciphertext_2.deserialize(buffer_ciphertext);
  ciphertext_2.decrypt(sess_key_2, *dec_key_2);


  /**************************************************************************/

  // Check if the session keys are equal
  if (memcmp(sess_key_0, sess_key_1, RLC_MD_LEN) == 0) {
    cout << "session_key_0 = session_key_1" << endl;
  }

  if (memcmp(sess_key_0, sess_key_2, RLC_MD_LEN) == 0) {
    cout << "session_key_0 = session_key_2" << endl;
    return;
  }

  cout << "Session key 0: FAILED" << endl;
}

void example_unlinkability_public_keys() {
  cout << "\n----------------> START : " << __func__ << endl;

  KPABE_DPVS kpabe(wl, bl);
  kpabe.setup();

  bn_t rand_1, rand_2;

  auto public_key  = kpabe.get_public_key();
  auto pk_unlink_1 = public_key.randomize(rand_1);
  auto pk_unlink_2 = public_key.randomize(rand_2);

  // Check validity of the derived public keys
  if (public_key.validate_derived_key(pk_unlink_1, rand_1) &&
      !public_key.validate_derived_key(pk_unlink_1, rand_2)) {
    cout << "First derived public key is valid" << endl;
  }
  else {
    cout << "Errors : the derived public key is invalid" << endl;
  }

  if (public_key.validate_derived_key(pk_unlink_2, rand_2) &&
      !public_key.validate_derived_key(pk_unlink_2, rand_1)) {
    cout << "Second derived public key is valid" << endl;
  }
  else {
    cout << "Errors : the derived public key is invalid" << endl;
  }

  bn_free(rand_1);
  bn_free(rand_2);
}

void example_unlinkability_decryption() {

  cout << "\n----------------> START : " << __func__ << endl;

  KPABE_DPVS kpabe(wl, bl);
  kpabe.setup();

  auto public_key = kpabe.get_public_key();
  auto master_key = kpabe.get_master_key();
  auto dec_key_00 = kpabe.keygen(policy_00);

  bn_t rand;
  auto pk_unlink = public_key.randomize(rand);

  // Generate two ciphertexts using public_key and pk_unlink
  KPABE_DPVS_CIPHERTEXT ciphertext("A1|A2|A5", "url");
  KPABE_DPVS_CIPHERTEXT ctx_unlink("A1|A2|A5", "url");

  // Generate a session key
  bn_t phi;
  uint8_t session_key[RLC_MD_LEN];
  generate_session_key(session_key, phi);

  // Encrypt the ciphertext using public_key and ctx_unlink using pk_unlink
  ciphertext.encrypt(phi, public_key);
  ctx_unlink.encrypt(phi, pk_unlink);

  // Verification : Decryption using dec_key_00
  uint8_t session_key_1[RLC_MD_LEN];
  uint8_t session_key_2[RLC_MD_LEN];
  uint8_t session_key_3[RLC_MD_LEN];

  bool b1, b2, b3;

  // 1. Decrypt the ciphertext --> must be successful
  b1 = ciphertext.decrypt(session_key_1, *dec_key_00);
  if (b1 == true) cout << "SUCCESS"; else cout << "FAILED";
  if (memcmp(session_key, session_key_1, RLC_MD_LEN) == 0) {
    cout << " --> Decryption of the ciphertext" << endl;
  }
  else {
    cout << " --> Errors: Decryption of the ciphertext" << endl;
  }

  // 2. Decrypt the ctx_unlink --> must be failed (sess_key_2 != sess_key)
  b2 = ctx_unlink.decrypt(session_key_2, *dec_key_00);
  if (b2 != true) cout << "FAILED"; else cout << "SUCCESS";
  if (memcmp(session_key, session_key_2, RLC_MD_LEN) != 0) {
    cout << " --> Decryption of the ctx_unlink, the recovered session key is different from original one" << endl;
  }
  else {
    cout << " --> Errors: Decryption of the ctx_unlink" << endl;
  }

  // 3. Decrypt the ctx_unlink after removing the randomization --> must be successful
  ctx_unlink.remove_scalar(rand);
  b3 = ctx_unlink.decrypt(session_key_3, *dec_key_00);
  if (b3 == true) cout << "SUCCESS"; else cout << "FAILED";
  if (memcmp(session_key, session_key_3, RLC_MD_LEN) == 0) {
    cout << " --> Decryption of the ctx_unlink after removing the randomization" << endl;
  }
  else {
    cout << " --> Errors: Decryption of the ctx_unlink after removing the randomization" << endl;
  }

  bn_free(rand);
  bn_free(phi);
}


bn_t Fq;

int main(int argc, char const *argv[])
{
  if (!init_libraries()) return 1; // Initialize the libraries

  example_generate_keys();

  example_encryption();

  example_decryption();

  example_serialization();

  example_unlinkability_public_keys();

  example_unlinkability_decryption();

  clean_libraries(); // Clean the libraries

  return 0;
}

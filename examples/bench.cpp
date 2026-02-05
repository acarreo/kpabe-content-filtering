/**
 * @file new_bench.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief 
 * @date 2024-03-27
 *
 */


#include <iostream>
#include <fstream>
#include <chrono>

#include "kpabe.hpp"

using namespace std;
using namespace std::chrono;


// generate a list of attributes with a given prefix and a number of attributes
vector<string> generateAttributesList(string prefix, int n) {
  vector<string> list;
  for (int i = 1; i <= n; i++) {
    list.push_back(prefix + to_string(i));
  }
  return list;
}

string generateAttributes(int n, int start = 1) {
  string attributes;
  for (int i = start; i < n+start; i++) {
    attributes += "Attr_" + to_string(i) + "|";
  }
  return attributes.substr(0, attributes.size() - 1);
}

size_t get_number_of_attributes_in_policy(const string& policy) {
  auto pol_tree = createPolicyTree(policy);
  auto attr_set = pol_tree->getAttrCompleteSet();

  return attr_set.size();
}

void print_number_of_attributes(const vector<string>& wl, const vector<string>& bl, const string& policy) {
  cout << "Number of attributes in White list ---> " << wl.size() << endl;
  cout << "Number of attributes in Black list ---> " << bl.size() << endl;
  cout << "Number of attributes in Policy -------> " << get_number_of_attributes_in_policy(policy) << endl;
}

void bench_serialization_params(int rounds)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking serialization of public and master keys -- Compression = " << BIN_COMPRESSED << "\n" << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup keys" << endl;
    return;
  }

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();


  // serialization of public key
  duration<double, std::micro> ser_pk_duration(0);
  duration<double, std::micro> ser_mk_duration(0);
  duration<double, std::micro> des_pk_duration(0);
  duration<double, std::micro> des_mk_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;

  for (int i = 0; i < rounds; i++) {
    ByteString public_key_bytes;
    ByteString master_key_bytes;

    KPABE_DPVS_PUBLIC_KEY public_key2;
    KPABE_DPVS_MASTER_KEY master_key2;

    t1 = high_resolution_clock::now();
    public_key.serialize(public_key_bytes);
    t2 = high_resolution_clock::now();
    public_key2.deserialize(public_key_bytes);
    t3 = high_resolution_clock::now();

    ser_pk_duration += duration_cast<microseconds>( t2 - t1 );
    des_pk_duration += duration_cast<microseconds>( t3 - t2 );
  
    t1 = high_resolution_clock::now();
    master_key.serialize(master_key_bytes);
    t2 = high_resolution_clock::now();
    master_key2.deserialize(master_key_bytes);
    t3 = high_resolution_clock::now();

    ser_mk_duration += duration_cast<microseconds>( t2 - t1 );
    des_mk_duration += duration_cast<microseconds>( t3 - t2 );

    if (!(public_key == public_key2) || !(master_key == master_key2) ||
        public_key.getSizeInBytes() != public_key_bytes.size() ||
        master_key.getSizeInBytes() != master_key_bytes.size()) {
      std::cerr << "Error: Public keys or master keys are not equal, or does not have the same length" << std::endl;
      return;
    }
  }
  cout << "Serialization of public key --> ByteString: " << (int) ser_pk_duration.count() / rounds << " us" << endl;
  cout << "Serialization of master key --> ByteString: " << (int) ser_mk_duration.count() / rounds << " us" << endl;
  cout << "-----------------------------------------------------------------------" << endl;
  cout << "Deserialization of public key <-- ByteString: " << (int) des_pk_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of master key <-- ByteString: " << (int) des_mk_duration.count() / rounds << " us" << endl;

  cout << "Size of public key: " << public_key.getSizeInBytes() << " bytes" << endl;
  cout << "Size of master key: " << master_key.getSizeInBytes() << " bytes" << endl;
}


void bench_serialization_params_iostream(int rounds)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking serialization of public and master keys -- Compression = " << BIN_COMPRESSED << "\n" << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup keys" << endl;
    return;
  }

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();

  duration<double, std::micro> ser_pk_duration(0);
  duration<double, std::micro> ser_mk_duration(0);
  duration<double, std::micro> des_pk_duration(0);
  duration<double, std::micro> des_mk_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;

  for (int i = 0; i < rounds; i++) {
    stringstream ss_public_key, ss_master_key;

    KPABE_DPVS_PUBLIC_KEY public_key2;
    KPABE_DPVS_MASTER_KEY master_key2;

    t1 = high_resolution_clock::now();
    public_key.serialize(ss_public_key);
    t2 = high_resolution_clock::now();
    public_key2.deserialize(ss_public_key);
    t3 = high_resolution_clock::now();

    ser_pk_duration += duration_cast<microseconds>( t2 - t1 );
    des_pk_duration += duration_cast<microseconds>( t3 - t2 );

    t1 = high_resolution_clock::now();
    master_key.serialize(ss_master_key);
    t2 = high_resolution_clock::now();
    master_key2.deserialize(ss_master_key);
    t3 = high_resolution_clock::now();

    ser_mk_duration += duration_cast<microseconds>( t2 - t1 );
    des_mk_duration += duration_cast<microseconds>( t3 - t2 );

    if (!(public_key == public_key2) || !(master_key == master_key2) ||
        public_key.getSizeInBytes() != ss_public_key.str().size() ||
        master_key.getSizeInBytes() != ss_master_key.str().size()) {
      std::cerr << "Error: Public keys or master keys are not equal, or does not have the same length" << std::endl;
      return;
    }
  }
  cout << "Serialization of public key --> iostream: " << (int) ser_pk_duration.count() / rounds << " us" << endl;
  cout << "Serialization of master key --> iostream: " << (int) ser_mk_duration.count() / rounds << " us" << endl;
  cout << "-----------------------------------------------------------------------" << endl;
  cout << "Deserialization of public key <-- iostream: " << (int) des_pk_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of master key <-- iostream: " << (int) des_mk_duration.count() / rounds << " us" << endl;

  cout << "Size of public key: " << public_key.getSizeInBytes() << " bytes" << endl;
  cout << "Size of master key: " << master_key.getSizeInBytes() << " bytes" << endl;
}

void bench_serialization_dec_key(int nb_wl, int nb_bl, int rounds)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking serialization of decryption key -- Compression = " << BIN_COMPRESSED << endl << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup keys" << endl;
    return;
  }

  vector<string> wl = generateAttributesList("WL_url_", nb_wl);
  vector<string> bl = generateAttributesList("BL_url_", nb_bl);
  string policy = "A5 and ((A1 and A2) or (A3 and A4))";

  auto dec_key = kpabe.keygen(policy, wl, bl);

  duration<double, std::micro> ser_dk_duration(0);
  duration<double, std::micro> des_dk_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3;

  for (int i = 0; i < rounds; i++) {
    ByteString dec_key_bytes;
    KPABE_DPVS_DECRYPTION_KEY dec_key2;

    t1 = high_resolution_clock::now();
    dec_key->serialize(dec_key_bytes);
    t2 = high_resolution_clock::now();
    dec_key2.deserialize(dec_key_bytes);
    t3 = high_resolution_clock::now();

    ser_dk_duration += duration_cast<microseconds>( t2 - t1 );
    des_dk_duration += duration_cast<microseconds>( t3 - t2 );

    if (!(*dec_key == dec_key2) || dec_key2.getSizeInBytes() != dec_key_bytes.size() ) {
      std::cerr << "Error: Decryption keys are not equal or does not have the same length" << std::endl;
      return;
    }
  }
  cout << "Serialization of decryption key ----> ByteString: " << (int) ser_dk_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of decryption key <-- ByteString: " << (int) des_dk_duration.count() / rounds << " us" << endl;

  cout << "-----------------------------------------------------------------------" << endl;
  cout << "Size of decryption key: " << dec_key->getSizeInBytes() << " bytes" << endl;
  print_number_of_attributes(wl, bl, policy);
}

void bench_encryption(int n_wl, int n_bl, int n_att, int rounds)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking encryption and decryption -- Compression = " << BIN_COMPRESSED << endl << endl;

  vector<string> wl = generateAttributesList("wl_url_", n_wl);
  vector<string> bl = generateAttributesList("bl_url_", n_bl);

  string policy = "Attr_1 and ((Attr_7 or Attr_5) and (Attr_4 or Attr_8))";
  string attributes_1 = generateAttributes(n_att);      // satisfy the policy
  string attributes_2 = generateAttributes(n_att, 10);  // does not satisfy the policy

  string url_in_wl = wl[wl.size()/2];
  string url_in_bl = bl[bl.size()/2];
  string url = "url_not_in_wl_and_bl";

  // cout << "--------- size wl and bl " << wl.size() << ", " << bl.size() << endl;
  // cout << url_in_wl << endl;
  // cout << url_in_bl << endl;
  // cout << attributes_1 << endl;
  // cout << attributes_2 << endl;


  uint8_t ss_key_1[RLC_MD_LEN], ss_key_1_rec[RLC_MD_LEN];
  uint8_t ss_key_2[RLC_MD_LEN], ss_key_2_rec[RLC_MD_LEN];
  uint8_t ss_key_3[RLC_MD_LEN], ss_key_3_rec[RLC_MD_LEN];
  uint8_t ss_key_4[RLC_MD_LEN], ss_key_4_rec[RLC_MD_LEN];

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup keys" << endl;
    return;
  }
  auto public_key = kpabe.get_public_key();
  auto dec_key = kpabe.keygen(policy, wl, bl);
  if (!dec_key) {
    cerr << "Error: Could not generate decryption key" << endl;
    return;
  }

  duration<double, std::micro> enc_duration(0);
  duration<double, std::micro> dec_duration(0);
  duration<double, std::micro> dec_duration_wl(0);
  duration<double, std::micro> dec_duration_bl(0);
  duration<double, std::micro> dec_duration_others(0);

  duration<double, std::micro> ser_ctx_duration(0);
  duration<double, std::micro> des_ctx_duration(0);

  chrono::time_point<chrono::high_resolution_clock> t1, t2, t3, t4;

  KPABE_DPVS_CIPHERTEXT ctx_1(attributes_1, url_in_wl),
                        ctx_2(attributes_1, url_in_bl),
                        ctx_3(attributes_1, url),
                        ctx_4(attributes_2, url);

  KPABE_DPVS_CIPHERTEXT ctx_deserialized;
  uint8_t des_ss_key[RLC_MD_LEN]; // to recover the session key from deserialized ciphertext

  for (int i = 0; i < rounds; i++)
  {
    {
      bool success = true;
      // Encryption time does not depend on url, so just compute it once and divide by 4 (nubber of ciphertexts)
      t1 = high_resolution_clock::now();
      success &= ctx_1.encrypt(ss_key_1, public_key);
      success &= ctx_2.encrypt(ss_key_2, public_key);
      success &= ctx_3.encrypt(ss_key_3, public_key);
      success &= ctx_4.encrypt(ss_key_4, public_key);
      t2 = high_resolution_clock::now();

      enc_duration += duration_cast<microseconds>( t2 - t1 );

      if (!success) {
        std::cerr << "Error: Could not encrypt" << std::endl;
        return;
      }

      // Decryption time depends on the url, so compute it for each ciphertext
      t1 = high_resolution_clock::now();
      success &= ctx_1.decrypt(ss_key_1_rec, *dec_key);    // url_in_wl
      t2 = high_resolution_clock::now();
      success &= !(ctx_2.decrypt(ss_key_2_rec, *dec_key)); // url_in_bl
      t3 = high_resolution_clock::now();
      success &= ctx_3.decrypt(ss_key_3_rec, *dec_key);    // attributes satisfies the policy when nb_att >= 5
      t4 = high_resolution_clock::now();
      success &= !(ctx_4.decrypt(ss_key_4_rec, *dec_key)); // attributes does not satisfy the policy

      dec_duration_others += duration_cast<microseconds>( high_resolution_clock::now() - t4 );
      dec_duration_wl += duration_cast<microseconds>( t2 - t1 ); // url_in_wl
      dec_duration_bl += duration_cast<microseconds>( t3 - t2 ); // url_in_bl
      dec_duration += duration_cast<microseconds>( t4 - t3 ); // url not in wl and bl

      // Check decryption results
      if (!success ||
          memcmp(ss_key_1, ss_key_1_rec, RLC_MD_LEN) != 0 ||
          memcmp(ss_key_2, ss_key_2_rec, RLC_MD_LEN) == 0 ||
          memcmp(ss_key_3, ss_key_3_rec, RLC_MD_LEN) != 0 ||
          memcmp(ss_key_4, ss_key_4_rec, RLC_MD_LEN) == 0)
      {
        std::cerr << "Error: Could not decrypt, success = " << success << std::endl;
        return;
      }

      // Serialization and deserialization of ciphertext
      ByteString ctx_bytes;
      t1 = high_resolution_clock::now();
      ctx_1.serialize(ctx_bytes);
      t2 = high_resolution_clock::now();
      ctx_deserialized.deserialize(ctx_bytes);
      t3 = high_resolution_clock::now();

      ser_ctx_duration += duration_cast<microseconds>( t2 - t1 );
      des_ctx_duration += duration_cast<microseconds>( t3 - t2 );

      // Check deserialization results
      if (ctx_1.getSizeInBytes() != ctx_bytes.size() ||
          !ctx_deserialized.decrypt(des_ss_key, *dec_key) ||
          memcmp(ss_key_1, des_ss_key, RLC_MD_LEN) != 0)
      {
        cerr << "Error: Could not deserialize" << endl;
        return;
      }
    }
  }

  cout << "Number of attributes ciphertext ------> " << n_att << endl;
  print_number_of_attributes(wl, bl, policy);
  cout << "-----------------------------------------------------------------------" << endl << endl;

  cout << "Size of ciphertexts " << ctx_1.getSizeInBytes() << " bytes" << endl;
  cout << "Serialization of ciphertexts ----> ByteString: " << (int) ser_ctx_duration.count() / rounds << " us" << endl;
  cout << "Deserialization of ciphertexts <-- ByteString: " << (int) des_ctx_duration.count() / rounds << " us" << endl;
  cout << "Encryption time -----------------> " << (int) enc_duration.count() / (rounds * 4) << " us" << endl << endl;

  cout << "Decryption time - url in wl ------> " << (int) dec_duration_wl.count() / rounds << " us" << endl;
  cout << "Decryption time - url in bl ------> " << (int) dec_duration_bl.count() / rounds << " us" << endl;
  cout << "Decryption time - url not in wl and wl (Policy satisfy) ------> " << (int) dec_duration.count() / (rounds * 4) << " us" << endl;
  cout << "Decryption time - url not in wl and bl (Policy not satisfy) --> " << (int) dec_duration_others.count() / rounds << " us" << endl;
}


void bench_time_generation_keys(int nb_wl, int nb_bl, int rounds)
{
  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking generation of decryption key -- Compression = " << BIN_COMPRESSED << endl << endl;

  KPABE_DPVS kpabe;
  if (!kpabe.setup()) {
    cerr << "Error: Could not setup keys" << endl;
    return;
  }

  vector<string> wl = generateAttributesList("WL_url_", nb_wl);
  vector<string> bl = generateAttributesList("BL_url_", nb_bl);

  string policy = "Attr_1 and ((Attr_7 or Attr_5) and (Attr_4 or Attr_8))";
  // string policy = "(Attr_1 and Attr_10) or ((Attr_7 or Attr_5) and (Attr_4 or Attr_8) and (Attr_2 or Attr_3) and (Attr_6 or Attr_9))";

  // duration<double, std::micro> params_duration(0);
  duration<double, std::micro> deckey_duration(0);
  chrono::time_point<chrono::high_resolution_clock> t1, t2;

  for (int i = 0; i < rounds; i++) {
    t1 = high_resolution_clock::now();
    auto dec_key = kpabe.keygen(policy, wl, bl);
    t2 = high_resolution_clock::now();

    deckey_duration += duration_cast<microseconds>( t2 - t1 );

    if (!dec_key) {
      cerr << "Error: Could not generate keys" << endl;
      return;
    }

    if (i == 0) {
      ByteString dec_key_bytes;
      dec_key->serialize(dec_key_bytes);
      if (dec_key_bytes.size() != dec_key->getSizeInBytes()) {
        cerr << "Error: Decryption key serialization failed" << endl;
        return;
      }
      cout << "Decryption key size -- ByteString -----> " << dec_key->getSizeInBytes() << " bytes" << endl;
    }
  }

  cout << "Number of attributes in White list ---> " << wl.size() << endl;
  cout << "Number of attributes in Black list ---> " << bl.size() << endl;
  cout << "Number of attributes in Policy -------> " << get_number_of_attributes_in_policy(policy) << endl;
  cout << "-----------------------------------------------------------------------" << endl;
  // cout << "Setup time -------------------------> " << (int) params_duration.count() / rounds << " us" << endl;
  cout << "Decryption key generation time -----> " << (int) deckey_duration.count() / rounds << " us" << endl;
}


int Nb_rounds = 100;
int n_wl  = 10;
int n_bl  = 10;
int n_att = 10;

int main(int argc, char **argv)
{
  InitializeOpenABE();

  if (argc == 4) {
    n_wl = atoi(argv[1]);
    n_bl = atoi(argv[2]);
    n_att = atoi(argv[3]);
  }

  cout << "\n----------------> START : " << __func__ << endl;
  cout << "Benchmarking KP-ABE with DPVS" << endl;
  cout << "Number of rounds: " << Nb_rounds << endl;

  cout << "Number of attributes in White list: " << n_wl << endl;
  cout << "Number of attributes in Black list: " << n_bl << endl;
  cout << "Number of attributes in ciphertext: " << n_att << endl << endl;

  bench_serialization_params_iostream(Nb_rounds);
  bench_serialization_params(Nb_rounds);

  bench_serialization_dec_key(n_wl, n_bl, Nb_rounds);

  // Il faut au moins 5 =: nb_att attributs dans le chiffré pour que le test passe correctement
  bench_encryption(n_wl, n_bl, n_att, Nb_rounds);

  bench_time_generation_keys(n_wl, n_bl, Nb_rounds);

  ShutdownOpenABE();

  return 0;
}

#include <iostream>
#include <fstream>
#include <chrono>

#include "kpabe/kpabe.hpp"
#include "keys/keys.hpp"

using namespace std;

bn_t Fq;

const vector<string> white_list({
  "wl_00", "wl_01", "wl_02", "wl_03", "wl_04", "wl_05", "wl_06", "wl_07", "wl_08", "wl_09",
  "wl_10", "wl_11", "wl_12", "wl_13", "wl_14", "wl_15", "wl_16", "wl_17", "wl_18", "wl_19",
  "wl_20", "wl_21", "wl_22", "wl_23", "wl_24", "wl_25", "wl_26", "wl_27", "wl_28", "wl_29",
  "wl_30", "wl_31", "wl_32", "wl_33", "wl_34", "wl_35", "wl_36", "wl_37", "wl_38", "wl_39",
  "wl_40", "wl_41", "wl_42", "wl_43", "wl_44", "wl_45", "wl_46", "wl_47", "wl_48", "wl_49",
  "wl_50", "wl_51", "wl_52", "wl_53", "wl_54", "wl_55", "wl_56", "wl_57", "wl_58", "wl_59",
  "wl_60", "wl_61", "wl_62", "wl_63", "wl_64", "wl_65", "wl_66", "wl_67", "wl_68", "wl_69",
  "wl_70", "wl_71", "wl_72", "wl_73", "wl_74", "wl_75", "wl_76", "wl_77", "wl_78", "wl_79",
  "wl_80", "wl_81", "wl_82", "wl_83", "wl_84", "wl_85", "wl_86", "wl_87", "wl_88", "wl_89",
  "wl_90", "wl_91", "wl_92", "wl_93", "wl_94", "wl_95", "wl_96", "wl_97", "wl_98", "wl_99",
});

const vector<string> black_list({
  "bl_00", "bl_01", "bl_02", "bl_03", "bl_04", "bl_05", "bl_06", "bl_07", "bl_08", "bl_09",
  "bl_10", "bl_11", "bl_12", "bl_13", "bl_14", "bl_15", "bl_16", "bl_17", "bl_18", "bl_19",
  "bl_20", "bl_21", "bl_22", "bl_23", "bl_24", "bl_25", "bl_26", "bl_27", "bl_28", "bl_29",
  "bl_30", "bl_31", "bl_32", "bl_33", "bl_34", "bl_35", "bl_36", "bl_37", "bl_38", "bl_39",
  "bl_40", "bl_41", "bl_42", "bl_43", "bl_44", "bl_45", "bl_46", "bl_47", "bl_48", "bl_49",
  "bl_50", "bl_51", "bl_52", "bl_53", "bl_54", "bl_55", "bl_56", "bl_57", "bl_58", "bl_59",
  "bl_60", "bl_61", "bl_62", "bl_63", "bl_64", "bl_65", "bl_66", "bl_67", "bl_68", "bl_69",
  "bl_70", "bl_71", "bl_72", "bl_73", "bl_74", "bl_75", "bl_76", "bl_77", "bl_78", "bl_79",
  "bl_80", "bl_81", "bl_82", "bl_83", "bl_84", "bl_85", "bl_86", "bl_87", "bl_88", "bl_89",
  "bl_90", "bl_91", "bl_92", "bl_93", "bl_94", "bl_95", "bl_96", "bl_97", "bl_98", "bl_99",
});

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

  KPABE_DPVS_PUBLIC_KEY public_key = kpabe.get_public_key();
  KPABE_DPVS_MASTER_KEY master_key = kpabe.get_master_key();

  KPABE_DPVS_PUBLIC_KEY public_key2;
  KPABE_DPVS_MASTER_KEY master_key2;


  // Test serialization of public and master keys

  ByteString public_key_bytes, master_key_bytes;
  CompressionType compress = BIN_COMPRESSED;

  public_key.serialize(public_key_bytes, compress);
  master_key.serialize(master_key_bytes, compress);

  // auto start = high_resolution_clock::now();
  // auto stop = high_resolution_clock::now();

  public_key2.deserialize(public_key_bytes);
  master_key2.deserialize(master_key_bytes);


  if (public_key == public_key2 && master_key == master_key2) {
    std::cout << "Public and master keys are equal" << std::endl;
  } else {
    std::cerr << "Error: Public and master keys are not equal" << std::endl;
  }

  // cout << "Time ----------> " << duration_cast<microseconds>(stop - start).count() << " us" << endl;
  // Test serialization of decryption key

  auto dec_key = kpabe.keygen("A5 and ((A1 and A2) or (A3 and A4))");
  KPABE_DPVS_DECRYPTION_KEY dec_key2;

  ByteString dec_key_bytes;
  dec_key->serialize(dec_key_bytes, compress);
  dec_key2.deserialize(dec_key_bytes);

  if (*dec_key == dec_key2) {
    std::cout << "Decryption keys are equal" << std::endl;
  } else {
    std::cerr << "Error: Decryption keys are not equal" << std::endl;
  }

  cout << "\nSizeof PK function ------> " << public_key.getSizeInBytes(compress) << endl;
  cout << "Public key size ---------> " << public_key_bytes.size() << endl;
  cout << "\nSizeof MK function ------> " << master_key.getSizeInBytes(compress) << endl;
  cout << "Master key size ---------> " << master_key_bytes.size() << endl;
  cout << "\nSizeof DK function ------> " << dec_key->getSizeInBytes(compress) << endl;
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
  CompressionType compress = BIN_COMPRESSED;
  cipher.serialize(cipher_bytes, compress);

  cout << "\nSizeof CT function ------> " << cipher.getSizeInBytes(compress) << endl;
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

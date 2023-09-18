#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

#include "../kpabe/kpabe.hpp"

using namespace std;

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

const vector<string> url_list({"url_00", "url_01", "url_02", "url_03", "url_04"});

const vector<string> list_attr_list({
  "A_00|A_01|A_02|A_03|A_04|A_05|A_06|A_07|A_08|A_09",
  "A_01|A_02|A_03|A_04|A_05",
  "A_02|A_06|A_08",
  "A_03|A_05|A_07",
  "A_00|A_01",
});

const vector<string> list_policies({
  "A_05 and (A_01 and A_02)",
  "(A_00 and A_01) or (A_03 and A_04)",
  "A_07 and (A_06 or A_05) and (A_04 or A_03)",
  "A_05 and (A_01 and A_02) or (A_03 and A_04)",
  "(A_07 or A_08) and (A_09 or A_06) and (A_01 or A_02)",
});

const list_decryption_key_file({ "/tmp/decryption_key_1",
        "/tmp/decryption_key_2", "/tmp/decryption_key_3",
        "/tmp/decryption_key_4", "/tmp/decryption_key_5",
});

vector<string> list_ciphertext_file;

// const string public_key_file("/tmp/public_key");
// const string master_key_file("/tmp/master_key");

KPABE_DPVS_PUBLIC_KEY public_key;
KPABE_DPVS_MASTER_KEY master_key;


void generate_decryption_key(string policy, string dec_key_file) {
  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, white_list, black_list);
  dec_key.generate(master_key).saveToFile(dec_key_file);
}

void encrypt(string &attr, string &url) {

  string tmp_filename = "/tmp/ciphertext__XXXXXX";

  int tmp_fd = mkstemp(&tmp_filename[0]);
  if (tmp_fd == -1) {
    cerr << "Error: creating ciphertext_filename failed" << endl;
    return;
  }

  close(tmp_fd);
  remove(tmp_filename.c_str());

  string ciphertext_filename = tmp_filename + "__" + url;
  list_ciphertext_file.push_back(ciphertext_filename);

  bn_t phi; bn_null(phi); bn_new(phi); bn_rand_mod(phi, Fq);

  KPABE_DPVS_CIPHERTEXT ciphertext(attr, url);
  ciphertext.encrypt(phi, public_key).saveToFile(ciphertext_filename);

  bn_free(phi);
}

bool decrypt(KPABE_DPVS_CIPHERTEXT &ciphertext, KPABE_DPVS_DECRYPTION_KEY &dec_key) {
  uint8_t session_key[32];
  return ciphertext.decrypt(session_key, dec_key);
}

bool decrypt(string ciphertext_filename, string dec_key_file) {
  KPABE_DPVS_CIPHERTEXT ciphertext(ciphertext_filename);
  KPABE_DPVS_DECRYPTION_KEY dec_key(dec_key_file);

  uint8_t session_key[32];
  // A voir plus tard. Il faut penser à sauvegarder le session key dans un
  // fichier de ciphertext pour pouvoir le verifier.

  return ciphertext.decrypt(session_key, dec_key)
}

void test_encrypt(void) {
  srand(static_cast<unsigned int>(time(nullptr)));

  vector<string> vect_attr(list_attr_list);
  random_shuffle(vect_attr.begin(), vect_attr.end());

  for (string& attr : vect_attr) {
    vector<string> selected_urls;
    selected_urls.push_back(white_list[rand() % white_list.size()]);
    selected_urls.push_back(black_list[rand() % black_list.size()]);
    selected_urls.push_back(url_list[rand() % url_list.size()]);

    for (string& url : selected_urls) {
      encrypt(attr, url);
    }
  }
}

void try_decrypt(void) {
  for (string& ciphertext_filename : list_ciphertext_file) {
    KPABE_DPVS_CIPHERTEXT ciphertext(ciphertext_filename);
    for (string& dec_key_file : list_decryption_key_file) {
      KPABE_DPVS_DECRYPTION_KEY dec_key(dec_key_file);
      uint8_t tmp_sess_key[32];
      if (ciphertext.decrypt(tmp_sess_key, dec_key)) {
        cout << "Decryption success" << endl;
      }
      else {
        cout << "Decryption failed" << endl;
      }
    }
  }
}


int main(int argc, char **argv) {
  if (!init_libraries()) return 1; // Initialize the libraries

  KPABE_DPVS kpabe(white_list, black_list);
  kpabe.setup();

  public_key = kpabe.get_public_key();
  master_key = kpabe.get_master_key();

  // Generate decryption keys and store them in list_decryption_key_file
  for (int i = 0; i < list_policies.size(); i++) {
    generate_decryption_key(list_policies[i], list_decryption_key_file[i]);
  }



  clean_libraries();
  return 0;
}



// call my_function with random attribut and url
// void my_function_2(int nb_url) {
//   vector<string> vect_BL(black_list);
//   vector<string> vect_WL(white_list);
//   vector<string> vect_URL(url_list);
//   vector<string> vect_attr(list_attr_list);

//   srand(static_cast<unsigned int>(time(nullptr)));

//   random_shuffle(vect_attr.begin(), vect_attr.end());
//   for (string& attr : vect_attr) {
//     random_shuffle(vect_WL.begin(), vect_WL.end());
//     random_shuffle(vect_BL.begin(), vect_BL.end());
//     random_shuffle(vect_URL.begin(), vect_URL.end());

//     vector<string> selected_urls;
//     selected_urls.insert(selected_urls.end(), vect_WL.begin(), vect_WL.begin() + nb_url);
//     selected_urls.insert(selected_urls.end(), vect_BL.begin(), vect_BL.begin() + nb_url);
//     selected_urls.insert(selected_urls.end(), vect_URL.begin(), vect_URL.begin() + nb_url);

//     for (string& url : selected_urls) {
//       my_function(attr, url);
//     }
//   }
// }

// string create_ciphertext_filename(const string& url) {
//   string temp_filename = "/tmp/ciphertext__XXXXXX";

//   int tmp_fd = mkstemp(&temp_filename[0]);
//   if (tmp_fd == -1)
//     return "";

//   close(tmp_fd);
//   remove(temp_filename.c_str());

//   return temp_filename + "__" + url;
// }
/**
 * @file keys.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief Keys classes for KP-ABE scheme using DPVS
 * @date 2023-08-14
 *
 */

#ifndef __KEYS_HPP__
#define __KEYS_HPP__

#include <algorithm>
#include <optional>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include <lsss/zlsss.h>

#include "../dpvs/vector_ec.hpp"

extern "C" {
  #include "../dpvs/dpvs.h"
  #include "../utils.h"
}

#define ND  3
#define NG  4
#define NF  6
#define NH  8

typedef enum KPABE_KEY_TYPE {
  KPABE_PUBLIC_KEY      = 0xFA,
  KPABE_MASTER_KEY      = 0xFB,
  KPABE_DECRYPTION_KEY  = 0xFC,
} KPABE_KEY_TYPE;

// Size of the hash of an attribute in base64 plus 2 (See hashAttribute function): "A:" + Base64(HASH(attribute))
// constexpr int SIZEOF_ATTRIBUTE = 9;                                         // in bytes
// constexpr int HASH_ATTRIBUTE_SIZE = ((SIZEOF_ATTRIBUTE + 2) / 3 * 4) + 2;   // in base64

#define HASH_ATTRIBUTE_SIZE   (((SIZEOF_ATTRIBUTE + 2) / 3 * 4) + 2)

class KPABE_DPVS_PUBLIC_KEY {
  public:
    KPABE_DPVS_PUBLIC_KEY() {};

    KPABE_DPVS_PUBLIC_KEY(const std::string filename) {
      if (access(filename.c_str(), F_OK) != -1)
        this->loadFromFile(filename);
    };

    ~KPABE_DPVS_PUBLIC_KEY() {};

    void set_bases(const G1_VS_BASE base_D, const G1_VS_BASE base_F,
                   const G1_VS_BASE base_G, const G1_VS_BASE base_H);

    // Getters
    G1_VECTOR get_d1() const { return this->d1; }
    G1_VECTOR get_d3() const { return this->d3; }
    G1_VECTOR get_f1() const { return this->f1; }
    G1_VECTOR get_f2() const { return this->f2; }
    G1_VECTOR get_f3() const { return this->f3; }
    G1_VECTOR get_g1() const { return this->g1; }
    G1_VECTOR get_g2() const { return this->g2; }
    G1_VECTOR get_h1() const { return this->h1; }
    G1_VECTOR get_h2() const { return this->h2; }
    G1_VECTOR get_h3() const { return this->h3; }

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    void serialize(std::vector<uint8_t>& buffer) const;
    void deserialize(const std::vector<uint8_t>& buffer);

    void serialize(ByteString &result, CompressionType compress) const;
    void deserialize(ByteString &input);

    size_t getSizeInBytes(CompressionType compress = BIN_COMPRESSED) const;

    void saveToFile(const std::string& filename) const {
      std::ofstream ofs(filename, std::ios::binary);
      if (ofs.is_open()) {
        this->serialize(ofs);
        ofs.close();
      }
    }

    void loadFromFile(const std::string& filename) {
      std::ifstream ifs(filename, std::ios::binary);
      if (ifs.is_open()) {
        this->deserialize(ifs);
        ifs.close();
      }
    }

    // Randomize the public key, the random scalar k is generated in the method
    std::pair<KPABE_DPVS_PUBLIC_KEY, ZP> randomize() const;

    // Check that a public key is derived from the current key: k * this == other ?
    bool validate_derived_key(const KPABE_DPVS_PUBLIC_KEY& other, const ZP k) const;

    // operator==
    bool operator==(const KPABE_DPVS_PUBLIC_KEY& other) const {
      return (this->d1 == other.d1 && this->d3 == other.d3 &&
              this->f1 == other.f1 && this->f2 == other.f2 && this->f3 == other.f3 &&
              this->g1 == other.g1 && this->g2 == other.g2 &&
              this->h1 == other.h1 && this->h2 == other.h2 && this->h3 == other.h3);
    }

  private:
    G1_VECTOR d1, d3;
    G1_VECTOR f1, f2, f3;
    G1_VECTOR g1, g2;
    G1_VECTOR h1, h2, h3;
};

class KPABE_DPVS_MASTER_KEY {
  public:
    KPABE_DPVS_MASTER_KEY() {};

    KPABE_DPVS_MASTER_KEY(const std::string filename) {
      if (access(filename.c_str(), F_OK) != -1)
        this->loadFromFile(filename);
    };

    ~KPABE_DPVS_MASTER_KEY() {};

    void set_bases(const G2_VS_BASE base_DD, const G2_VS_BASE base_FF,
                   const G2_VS_BASE base_GG, const G2_VS_BASE base_HH);

    // Getters
    G2_VECTOR get_dd1() const { return this->dd1; }
    G2_VECTOR get_dd3() const { return this->dd3; }
    G2_VECTOR get_ff1() const { return this->ff1; }
    G2_VECTOR get_ff2() const { return this->ff2; }
    G2_VECTOR get_ff3() const { return this->ff3; }
    G2_VECTOR get_gg1() const { return this->gg1; }
    G2_VECTOR get_gg2() const { return this->gg2; }
    G2_VECTOR get_hh1() const { return this->hh1; }
    G2_VECTOR get_hh2() const { return this->hh2; }
    G2_VECTOR get_hh3() const { return this->hh3; }

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    void serialize(ByteString &result, CompressionType compress) const;
    void deserialize(ByteString &input);

    void serialize(std::vector<uint8_t>& buffer) const;
    void deserialize(const std::vector<uint8_t>& buffer);

    size_t getSizeInBytes(CompressionType compress = BIN_COMPRESSED) const;

    void saveToFile(const std::string& filename) const {
      std::ofstream ofs(filename, std::ios::binary);
      if (ofs.is_open()) {
        this->serialize(ofs);
        ofs.close();
      }
    }

    void loadFromFile(const std::string& filename) {
      std::ifstream ifs(filename, std::ios::binary);
      if (ifs.is_open()) {
        this->deserialize(ifs);
        ifs.close();
      }
    }

    // operator==
    bool operator==(const KPABE_DPVS_MASTER_KEY& other) const {
      return (this->dd1 == other.dd1 && this->dd3 == other.dd3 &&
              this->ff1 == other.ff1 && this->ff2 == other.ff2 && this->ff3 == other.ff3 &&
              this->gg1 == other.gg1 && this->gg2 == other.gg2 &&
              this->hh1 == other.hh1 && this->hh2 == other.hh2 && this->hh3 == other.hh3);
    }

  private:
    G2_VECTOR dd1, dd3;
    G2_VECTOR ff1, ff2, ff3;
    G2_VECTOR gg1, gg2;
    G2_VECTOR hh1, hh2, hh3;
};

class KPABE_DPVS_DECRYPTION_KEY {
  public:
    typedef std::map<std::string, G2_VECTOR> key_map_t;

    KPABE_DPVS_DECRYPTION_KEY() : policy(""), white_list({}), black_list({}), hash_attributes(false) {};

    KPABE_DPVS_DECRYPTION_KEY(const std::string filename) {
      if (access(filename.c_str(), F_OK) != -1)
        this->loadFromFile(filename);
    };

    KPABE_DPVS_DECRYPTION_KEY(const std::string& policy_str,
                              const std::vector<std::string>& white_list,
                              const std::vector<std::string>& black_list,
                              bool hash_attr=false);

    ~KPABE_DPVS_DECRYPTION_KEY() {};

    /*
     * This method generates the decryption key from the master key and the
     * policy, white list and black list
     */
    bool generate(const KPABE_DPVS_MASTER_KEY& master_key);

    bool is_in_black_list(const std::string& url) const {
      return (std::find(this->black_list.begin(), this->black_list.end(), url)
              != this->black_list.end());
    }

    std::string get_policy() const { return this->policy; }

    // Method returning key_root
    G2_VECTOR get_key_root() const { return this->key_root; }

    // Get element of map key_wl by key : key_wl[url]
    std::optional<G2_VECTOR> get_key_wl(const std::string& url) const {
      auto it = this->key_wl.find(url);
      if (it != this->key_wl.end()) {
        return it->second;
      }
      return std::nullopt;
    }

    // Get element of map ket_att by key : key_att[att]
    std::optional<G2_VECTOR> get_key_att(const std::string& att) const {
      auto it = this->key_att.find(att);
      if (it != this->key_att.end()) {
        return it->second;
      }
      return std::nullopt;
    }

    // Methods to get an iterator to the beginning and end of the black list
    key_map_t::const_iterator get_key_bl_begin() const {
      return this->key_bl.begin();
    }
    key_map_t::const_iterator get_key_bl_end() const {
      return this->key_bl.end();
    }

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

    void serialize(ByteString &result, CompressionType compress) const;
    void deserialize(ByteString &input);

    void serialize(std::vector<uint8_t>& buffer) const;
    void deserialize(const std::vector<uint8_t>& buffer);

    size_t getSizeInBytes(CompressionType compress = BIN_COMPRESSED) const;

    void saveToFile(const std::string& filename) const {
      std::ofstream ofs(filename, std::ios::binary);
      if (ofs.is_open()) {
        this->serialize(ofs);
        ofs.close();
      }
    }

    void loadFromFile(const std::string& filename) {
      std::ifstream ifs(filename, std::ios::binary);
      if (ifs.is_open()) {
        this->deserialize(ifs);
        ifs.close();
      }
    }

    // operator==
    bool operator==(const KPABE_DPVS_DECRYPTION_KEY& other) const;

  private:
    std::string policy;
    std::vector<std::string> white_list;
    std::vector<std::string> black_list;
    bool hash_attributes;

    G2_VECTOR key_root;   // D*
    key_map_t key_wl;     // F*
    key_map_t key_bl;     // G*
    key_map_t key_att;    // H*
};

#endif // end of __KEYS_HPP__

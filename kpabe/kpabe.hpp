/**
 * @file kpabe.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief KP-ABE scheme using DPVS structure
 * @date 2023-08-16
 *
 */

#ifndef __KPABE_HPP__
#define __KPABE_HPP__

#include <algorithm>
#include <optional>
#include <vector>
#include <string>
#include <map>

#include "../keys/keys.hpp"


// Ciphertext class
class KPABE_DPVS_CIPHERTEXT {
  public:
    typedef std::map<std::string, G1_VECTOR> ctx_map_t;

    KPABE_DPVS_CIPHERTEXT() : attributes(""), url("") {};

    KPABE_DPVS_CIPHERTEXT(const std::string& attributes, const std::string& url) {
      this->attributes = attributes;
      this->url = url;
    };

    KPABE_DPVS_CIPHERTEXT(const std::string filename) {
      if (access(filename.c_str(), F_OK) != -1)
        this->loadFromFile(filename);
    };

    ~KPABE_DPVS_CIPHERTEXT() {};

    // Setters for attributes and url
    void set_attributes(const std::string& attributes) {
      this->attributes = attributes;
    }
    void set_url(const std::string& url) { this->url = url; }

    // Getters for attributes and url
    std::string get_attributes() const { return this->attributes; }
    std::string get_url() const { return this->url; }

    // Getters for G1 vectors members
    G1_VECTOR get_ctx_root() const { return this->ctx_root; }
    G1_VECTOR get_ctx_wl() const { return this->ctx_wl; }
    G1_VECTOR get_ctx_bl() const { return this->ctx_bl; }

    // Get element of map ctx_att by key : ctx_att[att]
    std::optional<G1_VECTOR> get_ctx_att(const std::string& att) const {
      auto it = this->ctx_att.find(att);
      if (it != this->ctx_att.end()) {
        return it->second;
      }
      return std::nullopt;
    }

    bool cipher(gt_t psi, const KPABE_DPVS_PUBLIC_KEY& public_key);

    void serialize(std::ostream& os) const;
    void deserialize(std::istream& is);

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

  private:
    std::string attributes;
    std::string url;

    G1_VECTOR ctx_root;   // D
    G1_VECTOR ctx_wl;     // F
    G1_VECTOR ctx_bl;     // G
    ctx_map_t ctx_att;    // H
};


/* Class for KP-ABE scheme using DPVS structure */
class KPABE_DPVS {
  public:
    KPABE_DPVS() {};

    KPABE_DPVS(const std::vector<std::string>& white_list,
               const std::vector<std::string>& black_list) {
      this->white_list = white_list;
      this->black_list = black_list;
    };
    
    ~KPABE_DPVS() {};

    // Setup : generate public and master keys
    bool setup();

    // Key generation, this method returns a decryption key
    std::optional<KPABE_DPVS_DECRYPTION_KEY> keygen(const std::string& policy) {
      KPABE_DPVS_DECRYPTION_KEY dec_key(policy, this->white_list, this->black_list);
      if (dec_key.generate(this->master_key)) {
        return dec_key;
      }
      return std::nullopt;
    }

    // Getter for public key
    KPABE_DPVS_PUBLIC_KEY get_public_key() const { return this->public_key; }

    // Export public key to file
    void export_public_key(const std::string& filename) const {
      this->public_key.saveToFile(filename);
    }

    // Export master key to file
    void export_master_key(const std::string& filename) const {
      this->master_key.saveToFile(filename);
    }

  private:
    std::vector<std::string> white_list;
    std::vector<std::string> black_list;

    KPABE_DPVS_PUBLIC_KEY public_key;
    KPABE_DPVS_MASTER_KEY master_key;
};

std::optional<KPABE_DPVS_CIPHERTEXT> encrypt(gt_t psi,
                                             const std::string& url,
                                             const std::string& attributes,
                                             const KPABE_DPVS_PUBLIC_KEY& public_key);

bool decrypt(gt_t phi,
             const KPABE_DPVS_CIPHERTEXT& ciphertext,
             const KPABE_DPVS_DECRYPTION_KEY& dec_key);

#endif // __KPABE_HPP__

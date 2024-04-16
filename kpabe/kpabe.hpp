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


#define KPABE_CIPHERTEXT_TYPE   0xFF

// Ciphertext class
class KPABE_DPVS_CIPHERTEXT : public Serializer<KPABE_DPVS_CIPHERTEXT> {
  public:
    typedef std::map<std::string, G1_VECTOR> ctx_map_t;

    KPABE_DPVS_CIPHERTEXT() : attributes(""), url(""), hash_attributes(false) {};

    KPABE_DPVS_CIPHERTEXT(const std::string& attributes, const std::string& url,
                          bool hash_attr = false) {
      this->hash_attributes = hash_attr;
      this->set_attributes(attributes);
      this->set_url(url);
    };

    KPABE_DPVS_CIPHERTEXT(const std::string filename) {
      if (access(filename.c_str(), F_OK) != -1)
        this->loadFromFile(filename);
    };

    ~KPABE_DPVS_CIPHERTEXT() {};

    // Setters for attributes and url
    void set_attributes(const std::string& attributes);
    void set_url(const std::string& url);

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

    // session_key is the output : it must be allocated before calling this method
    bool encrypt(uint8_t* session_key, const KPABE_DPVS_PUBLIC_KEY& public_key);

    bool decrypt(uint8_t* session_key,
                 const KPABE_DPVS_DECRYPTION_KEY& dec_key) const;

    // Remove k from the ciphertext : this = this * inverse(k)
    void remove_scalar(const ZP& k);

    size_t getSizeInBytes(CompressionType compress = BIN_COMPRESSED) const;

    void serialize(ByteString &result, CompressionType compress) const;
    void deserialize(ByteString &input);

    void serialize(std::ostream& os, CompressionType compress = BIN_COMPRESSED) const {
      this->serializeToStream(os, compress);
    }
    void deserialize(std::istream& is) {
      this->deserializeFromStream(is);
    }

    void serialize(std::vector<uint8_t>& bytes) const {
      this->serializeToBuffer(bytes);
    }
    void deserialize(const std::vector<uint8_t>& bytes) {
      this->deserializeFromBuffer(bytes);
    }

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
    bool hash_attributes;

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
               const std::vector<std::string>& black_list);

    ~KPABE_DPVS() {};

    // Setup : generate public and master keys
    bool setup();

    // Key generation, this method returns a decryption key
    std::optional<KPABE_DPVS_DECRYPTION_KEY> keygen(const std::string& policy) const {
      KPABE_DPVS_DECRYPTION_KEY dec_key(policy, this->white_list, this->black_list);
      if (dec_key.generate(this->master_key)) {
        return dec_key;
      }
      return std::nullopt;
    }

    std::optional<KPABE_DPVS_DECRYPTION_KEY> keygen(
                              const std::string& policy,
                              const std::vector<std::string>& white_list,
                              const std::vector<std::string>& black_list,
                              bool hash_attr = false) const;

    // Getter for public key
    KPABE_DPVS_PUBLIC_KEY get_public_key() const { return this->public_key; }

    // Getter for master key
    KPABE_DPVS_MASTER_KEY get_master_key() const { return this->master_key; }

    // Export public key to file
    void export_public_key(const std::string& filename) const {
      this->public_key.saveToFile(filename);
    }

    // Export master key to file
    void export_master_key(const std::string& filename) const {
      this->master_key.saveToFile(filename);
    }

    void export_public_key(std::vector<uint8_t>& bytes) const {
      this->public_key.serialize(bytes);
    }

    void export_master_key(std::vector<uint8_t>& bytes) const {
      this->master_key.serialize(bytes);
    }

  private:
    std::vector<std::string> white_list;
    std::vector<std::string> black_list;

    KPABE_DPVS_PUBLIC_KEY public_key;
    KPABE_DPVS_MASTER_KEY master_key;
};

#endif // __KPABE_HPP__

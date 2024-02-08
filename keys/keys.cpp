/**
 * @file keys.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of keys classes for KP-ABE scheme using DPVS
 * @date 2023-08-14
 *
 */

#include "keys.hpp"


void KPABE_DPVS_PUBLIC_KEY::set_bases(const G1_VS_BASE base_D,
                                      const G1_VS_BASE base_F,
                                      const G1_VS_BASE base_G,
                                      const G1_VS_BASE base_H)
{
  if (base_D[0]->dim == ND && base_F[0]->dim == NF &&
      base_G[0]->dim == NG && base_H[0]->dim == NH)
  {
    this->d1 = G1_VECTOR(base_D[0]);
    this->d3 = G1_VECTOR(base_D[2]);

    this->f1 = G1_VECTOR(base_F[0]);
    this->f2 = G1_VECTOR(base_F[1]);
    this->f3 = G1_VECTOR(base_F[2]);

    this->g1 = G1_VECTOR(base_G[0]);
    this->g2 = G1_VECTOR(base_G[1]);

    this->h1 = G1_VECTOR(base_H[0]);
    this->h2 = G1_VECTOR(base_H[1]);
    this->h3 = G1_VECTOR(base_H[2]);
  }
}

void KPABE_DPVS_PUBLIC_KEY::serialize(std::ostream &os) const {
  if (os.good()) {
    this->d1.serialize(os); this->d3.serialize(os);
    this->f1.serialize(os); this->f2.serialize(os); this->f3.serialize(os);
    this->g1.serialize(os); this->g2.serialize(os);
    this->h1.serialize(os); this->h2.serialize(os); this->h3.serialize(os);
  }
}

void KPABE_DPVS_PUBLIC_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    this->d1.deserialize(is); this->d3.deserialize(is);
    this->f1.deserialize(is); this->f2.deserialize(is); this->f3.deserialize(is);
    this->g1.deserialize(is); this->g2.deserialize(is);
    this->h1.deserialize(is); this->h2.deserialize(is); this->h3.deserialize(is);
  }
}

void KPABE_DPVS_PUBLIC_KEY::serialize(std::vector<uint8_t> &buffer) const {
  std::stringstream ss;
  this->serialize(ss);
  std::string str = ss.str();
  buffer.resize(str.size());
  std::copy(str.begin(), str.end(), buffer.begin());
}

void KPABE_DPVS_PUBLIC_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  std::stringstream ss;
  ss.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
  this->deserialize(ss);
}

KPABE_DPVS_PUBLIC_KEY KPABE_DPVS_PUBLIC_KEY::randomize(bn_t k) const
{
  bn_rand_mod(k, Fq);
  KPABE_DPVS_PUBLIC_KEY result;
  result.d1 = this->d1 * k; result.d3 = this->d3 * k;
  result.f1 = this->f1 * k; result.f2 = this->f2 * k; result.f3 = this->f3 * k;
  result.g1 = this->g1 * k; result.g2 = this->g2 * k;
  result.h1 = this->h1 * k; result.h2 = this->h2 * k; result.h3 = this->h3 * k;
  return result;
}

bool KPABE_DPVS_PUBLIC_KEY::validate_derived_key(const KPABE_DPVS_PUBLIC_KEY &other, const bn_t k) const
{
  return (this->d1 * k == other.d1 && this->d3 * k == other.d3 &&
          this->f1 * k == other.f1 && this->f2 * k == other.f2 && this->f3 * k == other.f3 &&
          this->g1 * k == other.g1 && this->g2 * k == other.g2 &&
          this->h1 * k == other.h1 && this->h2 * k == other.h2 && this->h3 * k == other.h3);
}

void KPABE_DPVS_MASTER_KEY::set_bases(const G2_VS_BASE base_DD,
                                      const G2_VS_BASE base_FF,
                                      const G2_VS_BASE base_GG,
                                      const G2_VS_BASE base_HH)
{
  if (base_DD[0]->dim == ND && base_FF[0]->dim == NF &&
      base_GG[0]->dim == NG && base_HH[0]->dim == NH)
  {
    this->dd1 = G2_VECTOR(base_DD[0]);
    this->dd3 = G2_VECTOR(base_DD[2]);

    this->ff1 = G2_VECTOR(base_FF[0]);
    this->ff2 = G2_VECTOR(base_FF[1]);
    this->ff3 = G2_VECTOR(base_FF[2]);

    this->gg1 = G2_VECTOR(base_GG[0]);
    this->gg2 = G2_VECTOR(base_GG[1]);

    this->hh1 = G2_VECTOR(base_HH[0]);
    this->hh2 = G2_VECTOR(base_HH[1]);
    this->hh3 = G2_VECTOR(base_HH[2]);
  }
}

void KPABE_DPVS_MASTER_KEY::serialize(std::ostream &os) const {
  if (os.good()) {
    this->dd1.serialize(os); this->dd3.serialize(os);
    this->ff1.serialize(os); this->ff2.serialize(os); this->ff3.serialize(os);
    this->gg1.serialize(os); this->gg2.serialize(os);
    this->hh1.serialize(os); this->hh2.serialize(os); this->hh3.serialize(os);
  }
}

void KPABE_DPVS_MASTER_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    this->dd1.deserialize(is); this->dd3.deserialize(is);
    this->ff1.deserialize(is); this->ff2.deserialize(is); this->ff3.deserialize(is);
    this->gg1.deserialize(is); this->gg2.deserialize(is);
    this->hh1.deserialize(is); this->hh2.deserialize(is); this->hh3.deserialize(is);
  }
}

void KPABE_DPVS_MASTER_KEY::serialize(std::vector<uint8_t> &buffer) const {
  std::stringstream ss;
  this->serialize(ss);
  std::string str = ss.str();
  buffer.resize(str.size());
  std::copy(str.begin(), str.end(), buffer.begin());
}

void KPABE_DPVS_MASTER_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  std::stringstream ss;
  ss.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
  this->deserialize(ss);
}

/**
 * @brief This method generates the decryption key, given the master key.
 *        Before calling this function, the policy, the white list and the
 *        black list must be set.
 *
 * @param master_key is the master secret key
 * @return true if the key is generated, false otherwise
 */
bool KPABE_DPVS_DECRYPTION_KEY::generate(const KPABE_DPVS_MASTER_KEY &master_key)
{
  bn_vect_t ri;
  bn_t y0, y1, y2, tmp1, tmp2;

  BPGroup group;
  OpenABELSSS lsss;


  if (this->policy.empty() || this->white_list.empty() || this->black_list.empty()) {
    std::cerr << "Error: Policy, white list or black list is empty" << std::endl;
    return false;
  }

  // Create policy tree
  auto policy_tree = createPolicyTree(this->policy);
  if (policy_tree == nullptr) {
    std::cerr << "Error: Could not create policy tree" << std::endl;
    return false;
  }

  bn_null(tmp1); bn_new(tmp1);
  bn_null(tmp2); bn_new(tmp2);

  bn_null(y1); bn_new(y1); bn_zero(y1); /* y1 := \sum_{i=1}^{|BL|} ri */
  bn_null(y0); bn_new(y0); bn_zero(y0); /* y0 := y1 + secret_y2 */
  bn_null(y2); bn_new(y2);

  // Generate random values for ri
  uint size_bl = this->black_list.size();
  bn_vect_init(ri, size_bl);
  for (uint i = 0; i < size_bl; i++) {
    bn_rand_mod(ri->coord[i], group.order);
    bn_mod_add(y1, y1, ri->coord[i], group.order);
  }

  // Generate random value for y2 and compute y0
  bn_rand_mod(y2, group.order);
  bn_mod_add(y0, y1, y2, group.order);

  // Share secret y2
  ZP secret_y2 = ZP(y2);
  lsss.shareSecret(policy_tree.get(), secret_y2);
  OpenABELSSSRowMap secret_shares = lsss.getRows();

  /* set key_root : -y0 * msk->dd1 + msk->dd3 */
  bn_neg(tmp1, y0); bn_mod(tmp1, tmp1, group.order);
  this->key_root = master_key.get_dd1() * tmp1 + master_key.get_dd3();

  /* set key_wl : msk->ff1 * (theta_j * url_j) + msk->ff2 * (-theta_j) + msk->ff3 * y0 */
  for (const auto& url_wl : this->white_list) {
    str_to_bn(tmp1, url_wl.c_str(), url_wl.size(), group.order); // url_j
    bn_rand_mod(tmp2, group.order);                              // theta_j
    bn_mod_mul(tmp1, tmp1, tmp2, group.order);                   // theta_j * url_j
    bn_neg(tmp2, tmp2); bn_mod(tmp2, tmp2, group.order);         // -theta_j

    this->key_wl[url_wl] = master_key.get_ff1() * tmp1 +
                           master_key.get_ff2() * tmp2 +
                           master_key.get_ff3() * y0;
  }

  /* set key_bl : msk->gg1 * (url_bl[i] * ri[i]) + msk->gg2 * (-ri[i]) */
  uint i = 0;
  for (const auto &url_bl : this->black_list) {
    str_to_bn(tmp1, url_bl.c_str(), url_bl.size(), group.order);
    bn_mod_mul(tmp1, tmp1, ri->coord[i], group.order);
    bn_neg(tmp2, ri->coord[i]); bn_mod(tmp2, tmp2, group.order);

    this->key_bl[url_bl] = master_key.get_gg1() * tmp1 + // msk->gg1 * (url_bl[i] * ri[i])
                           master_key.get_gg2() * tmp2;  // msk->gg2 * (-ri[i])

    i++;
  }

  /* set key_att : for all secret in secret_shares,
   * msk->hh1 * (att_j * theta_j) + msk->hh2 * (-theta_j) + msk->hh3 * aj */
  for (const auto& [_, secret] : secret_shares) {
    ZP aj = secret.element();
    std::string att = secret.label();

    str_to_bn(tmp1, att.c_str(), att.size(), group.order);      // att_j
    bn_rand_mod(tmp2, group.order);                             // theta_j
    bn_mod_mul(tmp1, tmp1, tmp2, group.order);                  // att_j * theta_j
    bn_neg(tmp2, tmp2); bn_mod(tmp2, tmp2, group.order);        // -theta_j

    this->key_att[att] = master_key.get_hh1() * tmp1 +
                         master_key.get_hh2() * tmp2 +
                         master_key.get_hh3() * aj.m_ZP;
  }

  // Clear memory
  bn_free(tmp1); bn_free(tmp2);
  bn_free(y0); bn_free(y1); bn_free(y2);
  bn_vect_clear(ri);

  return true;
}

void KPABE_DPVS_DECRYPTION_KEY::serialize(std::ostream &os) const {
 if (os.good()) {
  // write policy_size and policy in the output stream as bytes
  uint policy_size = this->policy.size();
  os.write(reinterpret_cast<const char*>(&policy_size), sizeof(policy_size));
  os.write(this->policy.c_str(), policy_size);

  /* We dont need to write white_list and black_list explicitly
   * because they are included in key_wl and key_bl respectively.
   * They must be recover during deserialization. */

  // write key_root
  this->key_root.serialize(os);

  // write key_wl.size() and key_wl
  uint key_wl_size = this->key_wl.size();
  os.write(reinterpret_cast<const char*>(&key_wl_size), sizeof(key_wl_size));
  for (const auto& [key, value] : this->key_wl) {
    uint key_size = key.size();
    os.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
    os.write(key.c_str(), key_size);
    value.serialize(os);
  }

  // write key_bl.size() and key_bl
  uint key_bl_size = this->key_bl.size();
  os.write(reinterpret_cast<const char*>(&key_bl_size), sizeof(key_bl_size));
  for (const auto& [key, value] : this->key_bl) {
    uint key_size = key.size();
    os.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
    os.write(key.c_str(), key_size);
    value.serialize(os);
  }

  // write key_att.size() and key_att
  uint key_att_size = this->key_att.size();
  os.write(reinterpret_cast<const char*>(&key_att_size), sizeof(key_att_size));
  for (const auto& [key, value] : this->key_att) {
    uint key_size = key.size();
    os.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
    os.write(key.c_str(), key_size);
    value.serialize(os);
  }
 }
}

void KPABE_DPVS_DECRYPTION_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    uint policy_size = 0;
    uint key_size = 0;

    // clear white_list and black_list
    this->white_list.clear();
    this->black_list.clear();

    // read policy_size and policy from the input stream
    is.read(reinterpret_cast<char*>(&policy_size), sizeof(policy_size));
    std::string policy;
    policy.resize(policy_size);
    is.read(&policy[0], policy_size);
    this->policy = policy;

    // read key_root
    this->key_root.deserialize(is);

    // read key_wl.size() and key_wl
    uint key_wl_size = 0;
    is.read(reinterpret_cast<char*>(&key_wl_size), sizeof(key_wl_size));
    for (uint i = 0; i < key_wl_size; i++) {
      is.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
      std::string key; key.resize(key_size);
      is.read(&key[0], key_size);

      this->key_wl[key].deserialize(is);
      this->white_list.push_back(key);
    }

    // read key_bl.size() and key_bl
    uint key_bl_size = 0;
    is.read(reinterpret_cast<char*>(&key_bl_size), sizeof(key_bl_size));
    for (uint i = 0; i < key_bl_size; i++) {
      is.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
      std::string key; key.resize(key_size);
      is.read(&key[0], key_size);

      this->key_bl[key].deserialize(is);
      this->black_list.push_back(key);
    }

    // read key_att.size() and key_att
    uint key_att_size = 0;
    is.read(reinterpret_cast<char*>(&key_att_size), sizeof(key_att_size));
    for (uint i = 0; i < key_att_size; i++) {
      is.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
      std::string key; key.resize(key_size);
      is.read(&key[0], key_size);

      this->key_att[key].deserialize(is);
    }
  }
}

void KPABE_DPVS_DECRYPTION_KEY::serialize(std::vector<uint8_t> &buffer) const {
  std::stringstream ss;
  this->serialize(ss);
  std::string str = ss.str();
  buffer.resize(str.size());
  std::copy(str.begin(), str.end(), buffer.begin());
}

void KPABE_DPVS_DECRYPTION_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  std::stringstream ss;
  ss.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
  this->deserialize(ss);
}

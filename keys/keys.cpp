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

  BPGroup group(OpenABE_NONE_ID);
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

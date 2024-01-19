/**
 * @file kpabe.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of KP-ABE scheme using DPVS structure
 * @date 2023-08-16
 *
 */

#include "kpabe.hpp"

/**
 * @brief This method generates the public and master keys.
 * 
 * @return true if the keys are generated successfully, false otherwise
 */
bool KPABE_DPVS::setup() {

  bool is_setup = false;

  // Generate DPVS bases
  dpvs_t *base_D = dpvs_generate_bases(ND);
  dpvs_t *base_F = dpvs_generate_bases(NF);
  dpvs_t *base_G = dpvs_generate_bases(NG);
  dpvs_t *base_H = dpvs_generate_bases(NH);

  if (base_D == nullptr || base_F == nullptr ||
      base_G == nullptr || base_H == nullptr) {
    std::cerr << "Error: Could not generate DPVS bases" << std::endl;
  }
  else { 
    // Set public key
    public_key.set_bases(base_D->base, base_F->base,
                         base_G->base, base_H->base);

    // Set master key
    master_key.set_bases(base_D->dual_base, base_F->dual_base,
                         base_G->dual_base, base_H->dual_base);

    is_setup = true;
  }

  // Clear bases
  dpvs_clear(base_D);
  dpvs_clear(base_F);
  dpvs_clear(base_G);
  dpvs_clear(base_H);

  return is_setup;
}

/**
 * @brief This method encrypts an ephemeral session key, using the public key,
 *        url and set of attributes. Before calling this function, the url and
 *        the attributes must be set. The ciphertext is also generated.
 *
 * @param[in] phi Seed for the ephemeral session key, it is the message to encrypt
 * @param[in] public_key The public key
 * @return true if the encryption is successful, false otherwise
 */
bool KPABE_DPVS_CIPHERTEXT::encrypt(const bn_t phi, const KPABE_DPVS_PUBLIC_KEY& public_key)
{
  bn_t sigma, omega, bn_url, tmp, sigma_att;
  BPGroup group;

  if (this->url.empty() || this->attributes.empty()) {
    std::cerr << "Error: URL or attributes are empty" << std::endl;
    return false;
  }

  bn_null(sigma);  bn_new(sigma);  bn_rand_mod(sigma, group.order);
  bn_null(omega);  bn_new(omega);  bn_rand_mod(omega, group.order);

  bn_null(sigma_att); bn_new(sigma_att);
  bn_null(bn_url);    bn_new(bn_url);
  bn_null(tmp);       bn_new(tmp);

  /* set ctx_root : pk->d1 * omega + pk->d3 * phi */
  this->ctx_root = public_key.get_d1() * omega +
                   public_key.get_d3() * phi;

  /* set ctx_wl : pk->f1 * sigma + pk->f2 * (sigma * bn_url) + pk->f3 * omega */
  str_to_bn(bn_url, this->url.c_str(), this->url.size(), group.order);
  bn_mod_mul(tmp, sigma, bn_url, group.order);
  this->ctx_wl = public_key.get_f1() * sigma +
                 public_key.get_f2() * tmp +
                 public_key.get_f3() * omega;

  /* set ctx_bl : pk->g1 * omega + (omega * bn_url) * pk->g2 */
  bn_mod_mul(tmp, omega, bn_url, group.order);
  this->ctx_bl = public_key.get_g1() * omega + public_key.get_g2() * tmp;

  // Create attribute list
  std::unique_ptr<OpenABEAttributeList> attrList = createAttributeList(this->attributes);
  const std::vector<std::string>* attributes_list = attrList->getAttributeList();

  /* set ctx_att: for all att in attributes_list,
   *  pk->h1 * sigma_att + pk->h2 * (sigma_att * att) + omega * pk->h3 */
  G1_VECTOR h3_times_omega = public_key.get_h3() * omega;
  for (const auto& att : *attributes_list) {
    bn_rand_mod(sigma_att, group.order);
    str_to_bn(tmp, att.c_str(), att.size(), group.order);
    bn_mod_mul(tmp, tmp, sigma_att, group.order);

    this->ctx_att[att] = public_key.get_h1() * sigma_att +
                         public_key.get_h2() * tmp +
                         h3_times_omega;
  }

  // Clear memory
  bn_free(sigma); bn_free(omega);
  bn_free(sigma_att); bn_free(bn_url); bn_free(tmp);

  return true;
}

void KPABE_DPVS_CIPHERTEXT::serialize(std::ostream &os) const {
  if (os.good()) {
    // Serialize url
    uint url_size = this->url.size();
    os.write(reinterpret_cast<const char*>(&url_size), sizeof(uint));
    os.write(this->url.c_str(), url_size);

    // Write ctx_root, ctx_wl and ctx_bl
    this->ctx_root.serialize(os);
    this->ctx_wl.serialize(os);
    this->ctx_bl.serialize(os);

    // Write ctx_att.size() and ctx_att
    uint ctx_att_size = this->ctx_att.size();
    os.write(reinterpret_cast<const char*>(&ctx_att_size), sizeof(uint));
    for (const auto& [att, ctx] : this->ctx_att) {
      uint att_size = att.size();
      os.write(reinterpret_cast<const char*>(&att_size), sizeof(uint));
      os.write(att.c_str(), att_size);
      ctx.serialize(os);
    }
  }
}

void KPABE_DPVS_CIPHERTEXT::deserialize(std::istream &is) {
  if (is.good()) {
    uint url_size;
    uint att_size;

    // Deserialize url
    is.read(reinterpret_cast<char*>(&url_size), sizeof(uint));
    std::string url; url.resize(url_size);
    is.read(&url[0], url_size);
    this->url = url;

    // Read ctx_root, ctx_wl and ctx_bl
    this->ctx_root.deserialize(is);
    this->ctx_wl.deserialize(is);
    this->ctx_bl.deserialize(is);

    // Read ctx_att.size() and ctx_att
    this->attributes.clear();
    uint ctx_att_size = 0;
    is.read(reinterpret_cast<char*>(&ctx_att_size), sizeof(uint));
    for (uint i = 0; i < ctx_att_size; i++) {
      is.read(reinterpret_cast<char*>(&att_size), sizeof(uint));
      std::string att; att.resize(att_size);
      is.read(&att[0], att_size);
      this->ctx_att[att].deserialize(is);
      this->attributes += att + "|";
    }
    this->attributes.pop_back(); // Delete last '|' character from attributes

    /* The attribute order may differ from the original order during
     * serialization, but this difference does not impact functionality. */
  }
}

void KPABE_DPVS_CIPHERTEXT::serialize(std::vector<uint8_t>& bytes) const {
  std::stringstream ss(std::ios::binary);
  this->serialize(ss);
  std::string s = ss.str();
  bytes.resize(s.size());
  std::copy(s.begin(), s.end(), bytes.begin());
}

void KPABE_DPVS_CIPHERTEXT::deserialize(const std::vector<uint8_t>& bytes) {
  std::stringstream ss(std::ios::binary);
  ss.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
  this->deserialize(ss);
}

/**
 * @brief This method try to decrypt the ciphertext, using the decryption key.
 *        The decryption fails if the url is in the black list or if the policy
 *        is not satisfied. If the decryption is successful, the session key is
 *        recovered.
 *
 * @param[out] session_key The recovered session key
 * @param[in]  ciphertext The ciphertext to decrypt
 * @param[in]  dec_key The decryption key
 * @return true if the decryption is successful, false otherwise 
 */
bool KPABE_DPVS_CIPHERTEXT::decrypt(uint8_t *session_key,
                                    const KPABE_DPVS_DECRYPTION_KEY &dec_key) const
{
  // TODO : clear memory at the end of the function, even in case of error

  bn_t bn, bn_bl, bn_url;
  gt_t ip, ip_lsss, ip_bl, ip_root;
  gt_t phi;

  bn_null(bn);     bn_new(bn);
  bn_null(bn_bl);  bn_new(bn_bl);
  bn_null(bn_url); bn_new(bn_url);

  gt_null(ip);      gt_new(ip);
  gt_null(ip_bl);   gt_new(ip_bl);
  gt_null(ip_lsss); gt_new(ip_lsss);
  gt_null(ip_root); gt_new(ip_root);
  gt_null(phi);     gt_new(phi);

  std::string url = this->url;

  auto key_wl_url = dec_key.get_key_wl(url);
  if (key_wl_url) {
    std::cout << "URL is in WHITE_LIST: " << url << std::endl;
    inner_product(ip, this->ctx_wl, *key_wl_url);
    inner_product(ip_root, this->ctx_root, dec_key.get_key_root());
    gt_mul(phi, ip, ip_root);
    gt_md_map(session_key, phi);

    return true;
  }

  if (dec_key.is_in_black_list(url)) {
    std::cout << "URL is blacklisted: " << url << std::endl;
    return false;
  }

  // Here, the url is not in WHITE_LIST and not in BLACK_LIST
  // std::cout << "URL is not in WHITE_LIST and not in BLACK_LIST: " << url << std::endl;


  BPGroup group;
  auto policy = createPolicyTree(dec_key.get_policy());
  auto attribute_list = createAttributeList(this->attributes);

  if (policy == nullptr || attribute_list == nullptr) {
    std::cerr << "Error: Could not create policy tree or attribute list" << std::endl;
    return false;
  }

  OpenABELSSS lsss;
  if (!lsss.recoverCoefficients(policy.get(), attribute_list.get())) {
    std::cout << "Policy not satisfied, could not recover LSSS coefficients." << std::endl;
    return false;
  }
  std::cout << "Policy satisfied, LSSS coefficients recovered successfully." << std::endl;

  auto recover_coeff = lsss.getRows();

  gt_set_unity(ip_lsss);
  for (const auto& [_, coeff] : recover_coeff) {
    ZP cj = coeff.element();
    std::string att = coeff.label();

    auto ctx_att__ = this->get_ctx_att(att);
    auto key_att__ = dec_key.get_key_att(att);

    if (!ctx_att__ || !key_att__) {
      std::cerr << "Error: Could not get ctx_att or key_att" << std::endl;
      return false;
    }

    inner_product(ip, *ctx_att__, *key_att__ * cj.m_ZP);
    gt_mul(ip_lsss, ip_lsss, ip);
  }

  str_to_bn(bn_url, url.c_str(), url.size(), group.order);
  gt_set_unity(ip_bl);
  for (auto it = dec_key.get_key_bl_begin(); it != dec_key.get_key_bl_end(); it++) {
    std::string bl = it->first;
    str_to_bn(bn_bl, bl.c_str(), bl.size(), group.order);
    bn_mod_sub(bn, bn_bl, bn_url, group.order);
    bn_mod_inv(bn, bn, group.order);

    inner_product(ip, this->ctx_bl, it->second);
    gt_exp(ip, ip, bn);
    gt_mul(ip_bl, ip_bl, ip);
  }

  inner_product(ip_root, this->ctx_root, dec_key.get_key_root());

  gt_mul(phi, ip_lsss, ip_bl);
  gt_mul(phi, phi, ip_root);

  gt_md_map(session_key, phi);

  // Clear memory
  bn_free(bn); bn_free(bn_bl); bn_free(bn_url);
  gt_free(ip); gt_free(ip_bl); gt_free(ip_lsss); gt_free(ip_root);
  gt_free(phi);

  return true;
}

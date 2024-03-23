/**
 * @file kpabe.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of KP-ABE scheme using DPVS structure
 * @date 2023-08-16
 *
 */

#include "kpabe.hpp"


/**
 * @brief Constructs a KPABE_DPVS object with the given white list and black list.
 *
 * @param white_list A vector of strings representing the attributes in the white list.
 * @param black_list A vector of strings representing the attributes in the black list.
 */
KPABE_DPVS::KPABE_DPVS(const std::vector<std::string>& white_list, const std::vector<std::string>& black_list)
{
  this->white_list.clear();
  this->black_list.clear();

  for (const auto& att : white_list) {
    this->white_list.push_back(hashAttribute(att));
  }

  for (const auto& att : black_list) {
    this->black_list.push_back(hashAttribute(att));
  }
}

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

std::optional<KPABE_DPVS_DECRYPTION_KEY> KPABE_DPVS::keygen(
                    const std::string& policy,
                    const std::vector<std::string>& white_list,
                    const std::vector<std::string>& black_list,
                    bool hash_attr) const
{
  KPABE_DPVS_DECRYPTION_KEY dec_key(policy, white_list, black_list, hash_attr);
  if (dec_key.generate(this->master_key)) {
    return dec_key;
  }
  return std::nullopt;
}

void KPABE_DPVS_CIPHERTEXT::set_attributes(const std::string &attributes) {
  if (this->hash_attributes) {
    this->attributes = hashAttributesList(attributes);
  }
  else {
    this->attributes = attributes;
  }
}

void KPABE_DPVS_CIPHERTEXT::set_url(const std::string &url) {
  if (this->hash_attributes) {
    this->url = hashAttribute(url);
  }
  else {
    this->url = url;
  }
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
bool KPABE_DPVS_CIPHERTEXT::encrypt(uint8_t* session_key, const KPABE_DPVS_PUBLIC_KEY& public_key)
{
  BPGroup group;
  ZP phi, sigma, omega;

  if (this->url.empty() || this->attributes.empty()) {
    std::cerr << "Error: URL or attributes are empty" << std::endl;
    return false;
  }

  phi.setRandom(group.order);
  sigma.setRandom(group.order);
  omega.setRandom(group.order);

  /* set ctx_root : pk->d1 * omega + pk->d3 * phi */
  this->ctx_root = public_key.get_d1() * omega +
                   public_key.get_d3() * phi;

  /* set ctx_wl : pk->f1 * sigma + pk->f2 * (sigma * url_zp) + pk->f3 * omega */
  ZP url_zp = hashToZP(this->url, group.order);
  this->ctx_wl = public_key.get_f1() * sigma +
                 public_key.get_f2() * (sigma * url_zp) +
                 public_key.get_f3() * omega;

  /* set ctx_bl : pk->g1 * omega + (omega * url_zp) * pk->g2 */
  this->ctx_bl = public_key.get_g1() * omega +
                 public_key.get_g2() * (omega * url_zp);

  // Create attribute list
  std::unique_ptr<OpenABEAttributeList> attributes_list = createAttributeList(this->attributes);
  const std::vector<std::string>* attrList = attributes_list->getAttributeList();

  /* set ctx_att: for all att in attributes_list,
   *  pk->h1 * sigma_att + pk->h2 * (sigma_att * att) + omega * pk->h3 */
  G1_VECTOR h3_times_omega = public_key.get_h3() * omega;
  for (const auto& att : *attrList) {
    ZP att_zp = hashToZP(att, group.order);
    sigma.setRandom(group.order); // sigma_att

    this->ctx_att[att] = public_key.get_h1() * sigma +
                         public_key.get_h2() * (sigma * att_zp) +
                         h3_times_omega;
  }

  // ---------------------------------> Generate session key
  G1 g1;  g1.setGenerator();
  G2 g2;  g2.setGenerator();

  GT gt = pairing(g1, g2).exp(phi);  // Ephemeral key : gt = e(g1, g2)^phi
  gt_md_map(session_key, gt.m_GT);
  // ---------------------------------> END Generate session key

  return true;
}

void KPABE_DPVS_CIPHERTEXT::serialize(ByteString& result, CompressionType compress) const {
  ByteString temp;

  result.insertFirstByte(KPABE_CIPHERTEXT_TYPE);

  temp.fromString(this->url);               result.smartPack(temp);
  this->ctx_root.serialize(temp, compress); result.smartPack(temp);
  this->ctx_wl.serialize(temp, compress);   result.smartPack(temp);
  this->ctx_bl.serialize(temp, compress);   result.smartPack(temp);

  uint16_t ctx_att_size = this->ctx_att.size();
  result.pack16bits(ctx_att_size);
  for (const auto& [att, ctx] : this->ctx_att) {
    temp.fromString(att);          result.smartPack(temp);
    ctx.serialize(temp, compress); result.smartPack(temp);
  }

  /* No need to serialize the attributes list, it is already serialized
   * int the ctx_att map.
  */
}

void KPABE_DPVS_CIPHERTEXT::deserialize(ByteString& input) {
  ByteString temp;
  size_t index = 0;
  std::string att;

  uint8_t element_type = input.at(index); index++;

  if (element_type != KPABE_CIPHERTEXT_TYPE) {
    std::cerr << "Error: Element type is not KPABE_CIPHERTEXT_TYPE" << std::endl;
    return;
  }

  temp = input.smartUnpack(&index); this->url = temp.toString();

  temp = input.smartUnpack(&index); this->ctx_root.deserialize(temp);
  temp = input.smartUnpack(&index); this->ctx_wl.deserialize(temp);
  temp = input.smartUnpack(&index); this->ctx_bl.deserialize(temp);

  std::string attributes;
  uint16_t ctx_att_size = input.get16bits(&index);
  for (uint16_t i = 0; i < ctx_att_size; i++) {
    temp = input.smartUnpack(&index); att = temp.toString();
    temp = input.smartUnpack(&index); this->ctx_att[att].deserialize(temp);
    attributes += att + "|";
  }
  this->attributes = attributes;

  /* The attribute order may differ from the original order during
   * serialization, but this difference does not impact functionality. */
}

void KPABE_DPVS_CIPHERTEXT::serialize(std::ostream &os) const {
  if (os.good()) {
    ByteString temp;
    size_t size = 0;

    this->serialize(temp, BIN_COMPRESSED);
    size = temp.size();

    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    os.write(reinterpret_cast<const char*>(temp.data()), static_cast<std::streamsize>(size));
  }
}

void KPABE_DPVS_CIPHERTEXT::deserialize(std::istream &is) {
  if (is.good()) {
    ByteString temp;
    size_t size = 0;

    is.read(reinterpret_cast<char*>(&size), sizeof(size));
    temp.fillBuffer(0, size);
    is.read(reinterpret_cast<char*>(temp.getInternalPtr()), static_cast<std::streamsize>(size));

    this->deserialize(temp);
  }
}

void KPABE_DPVS_CIPHERTEXT::serialize(std::vector<uint8_t>& bytes) const {
  ByteString temp;
  this->serialize(temp, BIN_COMPRESSED);
  bytes.resize(temp.size());
  std::copy(temp.data(), temp.data() + temp.size(), bytes.begin());
}

void KPABE_DPVS_CIPHERTEXT::deserialize(const std::vector<uint8_t>& bytes) {
  ByteString temp;
  temp.fillBuffer(0, bytes.size());
  std::copy(bytes.begin(), bytes.end(), temp.data());
  this->deserialize(temp);
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
  ZP zp, zp_bl, zp_url;
  GT ip, ip_lsss, ip_bl, ip_root;
  GT phi;

  std::string url = this->url;

  auto key_wl_url = dec_key.get_key_wl(url);
  if (key_wl_url) {
    std::cout << "URL is in WHITE_LIST: " << url << std::endl;
    ip = innerProduct(this->ctx_wl, *key_wl_url);
    ip_root = innerProduct(this->ctx_root, dec_key.get_key_root());
    phi = ip * ip_root;
    gt_md_map(session_key, phi.m_GT);

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
  auto attributes_list = createAttributeList(this->attributes);

  if (policy == nullptr || attributes_list == nullptr) {
    std::cerr << "Error: Could not create policy tree or attribute list" << std::endl;
    return false;
  }

  OpenABELSSS lsss;
  if (!lsss.recoverCoefficients(policy.get(), attributes_list.get())) {
    std::cout << "Policy not satisfied, could not recover LSSS coefficients." << std::endl;
    return false;
  }
  std::cout << "Policy satisfied, LSSS coefficients recovered successfully." << std::endl;

  auto recover_coeff = lsss.getRows();

  ip_lsss.setIdentity();
  for (const auto& [_, coeff] : recover_coeff) {
    ZP cj = coeff.element();
    std::string att = coeff.label();

    auto ctx_att__ = this->get_ctx_att(att);
    auto key_att__ = dec_key.get_key_att(att);

    if (!ctx_att__ || !key_att__) {
      std::cerr << "Error: Could not get ctx_att or key_att" << std::endl;
      return false;
    }

    ip = innerProduct(*ctx_att__, *key_att__ * cj);
    ip_lsss = ip_lsss * ip;
  }

  zp_url = hashToZP(url, group.order);
  ip_bl.setIdentity();
  for (auto it = dec_key.get_key_bl_begin(); it != dec_key.get_key_bl_end(); it++) {
    std::string bl = it->first;
    zp_bl = hashToZP(bl, group.order);
    zp = zp_bl - zp_url;
    zp.multInverse();

    ip = innerProduct(this->ctx_bl, it->second);
    ip_bl = ip_bl * ip.exp(zp);
  }

  ip_root = innerProduct(this->ctx_root, dec_key.get_key_root());

  phi = ip_lsss * ip_bl * ip_root;
  gt_md_map(session_key, phi.m_GT);

  return true;
}


/**
 * @brief This method removes the scalar `k` from the ciphertext, modifying it
 *        in place. It is used when the public key is randomized with the scalar `k`.
 *
 * @param k the scalar to remove from the ciphertext
 */
void KPABE_DPVS_CIPHERTEXT::remove_scalar(const ZP &k)
{
  ZP inv_k = ZP(k);
  inv_k.multInverse();

  this->ctx_root = this->ctx_root * inv_k;
  this->ctx_wl = this->ctx_wl * inv_k;
  this->ctx_bl = this->ctx_bl * inv_k;

  for (auto& [_, ctx] : this->ctx_att) {
    ctx = ctx * inv_k;
  }
}

size_t KPABE_DPVS_CIPHERTEXT::getSizeInBytes(CompressionType compress) const
{
  size_t total_size = 0;

  size_t surl = this->url.size();
  size_t sroot= this->ctx_root.getSizeInBytes(compress);
  size_t swl  = this->ctx_wl.getSizeInBytes(compress);
  size_t sbl  = this->ctx_bl.getSizeInBytes(compress);
  size_t satt = this->ctx_att.begin()->second.getSizeInBytes(compress);
  size_t att_s= HASH_ATTRIBUTE_SIZE + smart_sizeof(HASH_ATTRIBUTE_SIZE);

  total_size = (surl  + smart_sizeof(surl)) + (sroot + smart_sizeof(sroot)) +
               (swl   + smart_sizeof(swl))  + (sbl   + smart_sizeof(sbl)) +
               (satt + smart_sizeof(satt) + att_s + 1) * this->ctx_att.size() +
               sizeof(uint16_t) + sizeof(uint8_t);

  total_size +=1; // I don't know why should I add 1 to get the correct size

  return total_size;
}

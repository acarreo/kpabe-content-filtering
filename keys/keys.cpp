/**
 * @file keys.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of keys classes for KP-ABE scheme using DPVS
 * @date 2023-08-14
 *
 */

#include "keys.hpp"


/*****************************************************************************/
/*--------------------------- KPABE_DPVS_PUBLIC_KEY -------------------------*/
/*****************************************************************************/

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

std::pair<KPABE_DPVS_PUBLIC_KEY, ZP> KPABE_DPVS_PUBLIC_KEY::randomize() const
{
  KPABE_DPVS_PUBLIC_KEY result;
  BPGroup group;
  ZP rand;
  rand.setRandom(group.order);

  result.d1 = this->d1 * rand; result.d3 = this->d3 * rand;
  result.f1 = this->f1 * rand; result.f2 = this->f2 * rand; result.f3 = this->f3 * rand;
  result.g1 = this->g1 * rand; result.g2 = this->g2 * rand;
  result.h1 = this->h1 * rand; result.h2 = this->h2 * rand; result.h3 = this->h3 * rand;

  return std::make_pair(result, rand);
}

bool KPABE_DPVS_PUBLIC_KEY::validate_derived_key(const KPABE_DPVS_PUBLIC_KEY &other, const ZP k) const
{
  return (this->d1 * k == other.d1 && this->d3 * k == other.d3 &&
          this->f1 * k == other.f1 && this->f2 * k == other.f2 && this->f3 * k == other.f3 &&
          this->g1 * k == other.g1 && this->g2 * k == other.g2 &&
          this->h1 * k == other.h1 && this->h2 * k == other.h2 && this->h3 * k == other.h3);
}

void KPABE_DPVS_PUBLIC_KEY::serialize(ByteString &output, CompressionType compress) const {
  ByteString temp, result;

  result.insertFirstByte(KPABE_PUBLIC_KEY);

  this->d1.serialize(temp, compress); result.smartPack(temp);
  this->d3.serialize(temp, compress); result.smartPack(temp);

  this->f1.serialize(temp, compress); result.smartPack(temp);
  this->f2.serialize(temp, compress); result.smartPack(temp);
  this->f3.serialize(temp, compress); result.smartPack(temp);

  this->g1.serialize(temp, compress); result.smartPack(temp);
  this->g2.serialize(temp, compress); result.smartPack(temp);

  this->h1.serialize(temp, compress); result.smartPack(temp);
  this->h2.serialize(temp, compress); result.smartPack(temp);
  this->h3.serialize(temp, compress); result.smartPack(temp);

  // output.smartPack(result);
  result.serialize(output);
}

void KPABE_DPVS_PUBLIC_KEY::deserialize(ByteString &input) {
  ByteString temp;
  size_t index = 0;

  if (input.at(index++) != BYTESTRING || input.size() - input.get32bits(&index) < hdrLen) {
    std::cerr << "Error: Invalid input" << std::endl;
    return;
  }

  if (input.at(index++) != KPABE_PUBLIC_KEY) {
    std::cerr << "Error: Invalid public key type" << std::endl;
    return;
  }

  temp = input.smartUnpack(&index); this->d1.deserialize(temp);
  temp = input.smartUnpack(&index); this->d3.deserialize(temp);

  temp = input.smartUnpack(&index); this->f1.deserialize(temp);
  temp = input.smartUnpack(&index); this->f2.deserialize(temp);
  temp = input.smartUnpack(&index); this->f3.deserialize(temp);

  temp = input.smartUnpack(&index); this->g1.deserialize(temp);
  temp = input.smartUnpack(&index); this->g2.deserialize(temp);

  temp = input.smartUnpack(&index); this->h1.deserialize(temp);
  temp = input.smartUnpack(&index); this->h2.deserialize(temp);
  temp = input.smartUnpack(&index); this->h3.deserialize(temp);
}

size_t KPABE_DPVS_PUBLIC_KEY::getSizeInBytes(CompressionType compress) const {
  size_t total_size = hdrLen;

  size_t sd1 = this->d1.getSizeInBytes(compress);
  size_t sf1 = this->f1.getSizeInBytes(compress);
  size_t sg1 = this->g1.getSizeInBytes(compress);
  size_t sh1 = this->h1.getSizeInBytes(compress);

  total_size +=(sd1 + smart_sizeof(sd1)) * 2 + (sf1 + smart_sizeof(sf1)) * 3 +
               (sg1 + smart_sizeof(sg1)) * 2 + (sh1 + smart_sizeof(sh1)) * 3;

  total_size += sizeof(uint8_t); // size of key type

  return total_size;
}

void KPABE_DPVS_PUBLIC_KEY::serialize(std::ostream &os, CompressionType compress) const {
  if (os.good()) {
    ByteString temp;
    this->serialize(temp, compress);
    os.write(reinterpret_cast<const char*>(temp.data()), static_cast<std::streamsize>(temp.size()));
  }
}

void KPABE_DPVS_PUBLIC_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    ByteString temp, bytes;
    size_t size = 0;

    if (!getSizeFromStream(is, &size, bytes)) {
      std::cerr << "Error: Could not get size from stream" << std::endl;
      return;
    }

    temp.fillBuffer(0, size);
    is.read(reinterpret_cast<char*>(temp.getInternalPtr()), static_cast<std::streamsize>(size));
    if (!is.good()) {
      std::cerr << "Error: Could not read data" << std::endl;
      return;
    }

    bytes += temp;
    this->deserialize(bytes);
  }
}

void KPABE_DPVS_PUBLIC_KEY::serialize(std::vector<uint8_t> &buffer) const {
  std::cout << "Serializing public key" << std::endl;
  ByteString temp;
  this->serialize(temp, BIN_COMPRESSED);
  buffer.resize(temp.size());
  std::copy(temp.data(), temp.data() + temp.size(), buffer.begin());
}

void KPABE_DPVS_PUBLIC_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  ByteString temp;
  temp.fillBuffer(0, buffer.size());
  std::copy(buffer.begin(), buffer.end(), temp.data());
  this->deserialize(temp);
}


/*****************************************************************************/
/*--------------------------- KPABE_DPVS_MASTER_KEY -------------------------*/
/*****************************************************************************/

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

void KPABE_DPVS_MASTER_KEY::serialize(ByteString &output, CompressionType compress) const {
  ByteString temp, result;

  result.insertFirstByte(KPABE_MASTER_KEY);

  this->dd1.serialize(temp, compress); result.smartPack(temp);
  this->dd3.serialize(temp, compress); result.smartPack(temp);

  this->ff1.serialize(temp, compress); result.smartPack(temp);
  this->ff2.serialize(temp, compress); result.smartPack(temp);
  this->ff3.serialize(temp, compress); result.smartPack(temp);

  this->gg1.serialize(temp, compress); result.smartPack(temp);
  this->gg2.serialize(temp, compress); result.smartPack(temp);

  this->hh1.serialize(temp, compress); result.smartPack(temp);
  this->hh2.serialize(temp, compress); result.smartPack(temp);
  this->hh3.serialize(temp, compress); result.smartPack(temp);

  result.serialize(output);
}

void KPABE_DPVS_MASTER_KEY::deserialize(ByteString &input) {
  ByteString temp;
  size_t index = 0;

  if (input.at(index++) != BYTESTRING || input.size() - input.get32bits(&index) < hdrLen) {
    std::cerr << "Error: Invalid input" << std::endl;
    return;
  }

  if (input.at(index++) != KPABE_MASTER_KEY) {
    std::cerr << "Error: Invalid master key type" << std::endl;
    return;
  }

  temp = input.smartUnpack(&index); this->dd1.deserialize(temp);
  temp = input.smartUnpack(&index); this->dd3.deserialize(temp);

  temp = input.smartUnpack(&index); this->ff1.deserialize(temp);
  temp = input.smartUnpack(&index); this->ff2.deserialize(temp);
  temp = input.smartUnpack(&index); this->ff3.deserialize(temp);

  temp = input.smartUnpack(&index); this->gg1.deserialize(temp);
  temp = input.smartUnpack(&index); this->gg2.deserialize(temp);

  temp = input.smartUnpack(&index); this->hh1.deserialize(temp);
  temp = input.smartUnpack(&index); this->hh2.deserialize(temp);
  temp = input.smartUnpack(&index); this->hh3.deserialize(temp);
}

void KPABE_DPVS_MASTER_KEY::serialize(std::ostream &os, CompressionType compress) const {
  if (os.good()) {
    ByteString temp;
    this->serialize(temp, compress);
    os.write(reinterpret_cast<const char*>(temp.data()), static_cast<std::streamsize>(temp.size()));
  }
}

void KPABE_DPVS_MASTER_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    ByteString temp, bytes;
    size_t size = 0;

    if (!getSizeFromStream(is, &size, bytes)) {
      std::cerr << "Error: Could not get size from stream" << std::endl;
      return;
    }

    temp.fillBuffer(0, size);
    is.read(reinterpret_cast<char*>(temp.getInternalPtr()), static_cast<std::streamsize>(size));
    if (!is.good()) {
      std::cerr << "Error: Could not read data" << std::endl;
      return;
    }

    bytes += temp;
    this->deserialize(bytes);
  }
}

void KPABE_DPVS_MASTER_KEY::serialize(std::vector<uint8_t> &buffer) const {
  ByteString temp;
  this->serialize(temp, BIN_COMPRESSED);
  buffer.resize(temp.size());
  std::copy(temp.data(), temp.data() + temp.size(), buffer.begin());
}

void KPABE_DPVS_MASTER_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  ByteString temp;
  temp.fillBuffer(0, buffer.size());
  std::copy(buffer.begin(), buffer.end(), temp.data());
  this->deserialize(temp);
}

size_t KPABE_DPVS_MASTER_KEY::getSizeInBytes(CompressionType compress) const {
  size_t total_size = 0;

  size_t sd1 = this->dd1.getSizeInBytes(compress);
  size_t sf1 = this->ff1.getSizeInBytes(compress);
  size_t sg1 = this->gg1.getSizeInBytes(compress);
  size_t sh1 = this->hh1.getSizeInBytes(compress);

  total_size = (sd1 + smart_sizeof(sd1)) * 2 + (sf1 + smart_sizeof(sf1)) * 3 +
               (sg1 + smart_sizeof(sg1)) * 2 + (sh1 + smart_sizeof(sh1)) * 3;

  total_size += sizeof(uint8_t); // size of key type  

  return total_size;
}

/*****************************************************************************/
/*------------------------ KPABE_DPVS_DECRYPTION_KEY ------------------------*/
/*****************************************************************************/

KPABE_DPVS_DECRYPTION_KEY::KPABE_DPVS_DECRYPTION_KEY(const std::string &policy_str,
                                                     const std::vector<std::string> &white_list,
                                                     const std::vector<std::string> &black_list,
                                                     bool hash_attr)
{
  this->hash_attributes = hash_attr;

  if (this->hash_attributes) {
    for (const auto &att : white_list) {
      this->white_list.push_back(hashAttribute(att));
    }

    for (const auto &att : black_list) {
      this->black_list.push_back(hashAttribute(att));
    }

    this->policy = hashPolicy(policy_str);
  }
  else {
    this->white_list = white_list;
    this->black_list = black_list;
    this->policy = policy_str;
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
  BPGroup group;
  OpenABELSSS lsss;

  ZP url, aj, att_j, theta_j;
  ZP r;
  std::vector<ZP> ri;

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

  // Generate random values for ri
  uint size_bl = this->black_list.size();

  ZP y1 = ZP((uint32_t)0);
  for (uint i = 0; i < size_bl; i++) {
    r.setRandom(group.order);
    ri.push_back(r);
    y1 += r;
  }


  // Generate random value for y2 and compute y0 := y1 + y2
  ZP y0, secret_y2;
  secret_y2.setRandom(group.order);
  y0 = y1 + secret_y2;

  // Share secret y2
  lsss.shareSecret(policy_tree.get(), secret_y2);
  OpenABELSSSRowMap secret_shares = lsss.getRows();

  /* set key_root : -y0 * msk->dd1 + msk->dd3 */
  this->key_root = master_key.get_dd1() * (-y0) + master_key.get_dd3();

  /* set key_wl : msk->ff1 * (theta_j * url_j) + msk->ff2 * (-theta_j) + msk->ff3 * y0 */
  for (const auto& url_wl : this->white_list) {
    url = hashToZP(url_wl, group.order);
    theta_j.setRandom(group.order);

    this->key_wl[url_wl] = master_key.get_ff1() * (theta_j * url) +
                           master_key.get_ff2() * (-theta_j) +
                           master_key.get_ff3() * y0;
  }

  /* set key_bl : msk->gg1 * (url_bl[i] * ri[i]) + msk->gg2 * (-ri[i]) */
  uint i = 0;
  for (const auto &url_bl : this->black_list) {
    url = hashToZP(url_bl, group.order);

    this->key_bl[url_bl] = master_key.get_gg1() * (url * ri.at(i)) +
                           master_key.get_gg2() * (-ri.at(i));

    i++;
  }

  /* set key_att : for all secret in secret_shares,
   * msk->hh1 * (att_j * theta_j) + msk->hh2 * (-theta_j) + msk->hh3 * aj */
  for (const auto& [_, secret] : secret_shares) {
    aj = secret.element(); aj.setOrder(group.order);
    std::string att = secret.label();

    att_j = hashToZP(att, group.order);
    theta_j.setRandom(group.order);

    this->key_att[att] = master_key.get_hh1() * (att_j * theta_j) +
                         master_key.get_hh2() * (-theta_j) +
                         master_key.get_hh3() * aj;
  }

  return true;
}

void KPABE_DPVS_DECRYPTION_KEY::serialize(ByteString &output, CompressionType compress) const {
  ByteString temp, result;

  result.insertFirstByte(KPABE_DECRYPTION_KEY);

  temp.fromString(this->policy);            result.smartPack(temp);
  this->key_root.serialize(temp, compress); result.smartPack(temp);

  uint16_t key_wl_size = this->key_wl.size();
  result.pack16bits(key_wl_size);
  for (const auto& [key, value] : this->key_wl) {
    temp.fromString(key);            result.smartPack(temp);
    value.serialize(temp, compress); result.smartPack(temp);
  }

  uint16_t key_bl_size = this->key_bl.size();
  result.pack16bits(key_bl_size);
  for (const auto& [key, value] : this->key_bl) {
    temp.fromString(key);            result.smartPack(temp);
    value.serialize(temp, compress); result.smartPack(temp);
  }

  uint16_t key_att_size = this->key_att.size();
  result.pack16bits(key_att_size);
  for (const auto& [key, value] : this->key_att) {
    temp.fromString(key);            result.smartPack(temp);
    value.serialize(temp, compress); result.smartPack(temp);
  }

  result.serialize(output);
}

void KPABE_DPVS_DECRYPTION_KEY::deserialize(ByteString &input) {
  ByteString temp;
  size_t index = 0;
  std::string key_str;

  // S'il n'y avait pas le cas 'oversized' dans custom stream, on aurait pu faire
  // bytes.deserialize(input); // deserialize the input in bytes
  // Et utiliser bytes (à la place de input) pour le reste du code
  if (input.at(index++) != BYTESTRING || input.size() - input.get32bits(&index) < hdrLen) {
    std::cerr << "Error: Invalid input" << std::endl;
    return;
  }

  if (input.at(index++) != KPABE_DECRYPTION_KEY) {
    std::cerr << "Error: Invalid decryption key type" << std::endl;
    return;
  }

  temp = input.smartUnpack(&index);
  this->policy = temp.toString();
  temp = input.smartUnpack(&index); this->key_root.deserialize(temp);

  uint16_t key_wl_size = input.get16bits(&index);
  for (uint16_t i = 0; i < key_wl_size; i++) {
    temp = input.smartUnpack(&index); key_str = temp.toString();
    temp = input.smartUnpack(&index); this->key_wl[key_str].deserialize(temp);
  }

  uint16_t key_bl_size = input.get16bits(&index);
  for (uint16_t i = 0; i < key_bl_size; i++) {
    temp = input.smartUnpack(&index); key_str = temp.toString();
    temp = input.smartUnpack(&index); this->key_bl[key_str].deserialize(temp);
  }

  uint16_t key_att_size = input.get16bits(&index);
  for (uint16_t i = 0; i < key_att_size; i++) {
    temp = input.smartUnpack(&index); key_str = temp.toString();
    temp = input.smartUnpack(&index); this->key_att[key_str].deserialize(temp);
  }
}

void KPABE_DPVS_DECRYPTION_KEY::serialize(std::ostream &os) const {
 if (os.good()) {
  ByteString temp;
  this->serialize(temp, BIN_COMPRESSED);
  os.write(reinterpret_cast<const char*>(temp.data()), static_cast<std::streamsize>(temp.size()));
 }
}

void KPABE_DPVS_DECRYPTION_KEY::deserialize(std::istream &is) {
  if (is.good()) {
    ByteString temp, bytes;
    size_t size = 0;

    if (!getSizeFromStream(is, &size, bytes)) {
      std::cerr << "Error: Could not get size from stream" << std::endl;
      return;
    }

    temp.fillBuffer(0, size);
    is.read(reinterpret_cast<char*>(temp.getInternalPtr()), static_cast<std::streamsize>(size));
    if (!is.good()) {
      std::cerr << "Error: Could not read data" << std::endl;
      return;
    }
    // temp.serialize(bytes); // serialize the temp in bytes

    bytes += temp;
    this->deserialize(bytes);
  }
}

void KPABE_DPVS_DECRYPTION_KEY::serialize(std::vector<uint8_t> &buffer) const {
  ByteString temp;
  this->serialize(temp, BIN_COMPRESSED);
  buffer.resize(temp.size());
  std::copy(temp.data(), temp.data() + temp.size(), buffer.begin());
}

void KPABE_DPVS_DECRYPTION_KEY::deserialize(const std::vector<uint8_t> &buffer) {
  ByteString temp;
  temp.fillBuffer(0, buffer.size());
  std::copy(buffer.begin(), buffer.end(), temp.data());
  this->deserialize(temp);
}

size_t KPABE_DPVS_DECRYPTION_KEY::getSizeInBytes(CompressionType compress) const
{
  size_t total_size = hdrLen;

  size_t spol = this->policy.size();
  size_t skr  = this->key_root.getSizeInBytes(compress);
  size_t skwl = this->key_wl.begin()->second.getSizeInBytes(compress);
  size_t skbl = this->key_bl.begin()->second.getSizeInBytes(compress);
  size_t skatt= this->key_att.begin()->second.getSizeInBytes(compress);

  size_t s_wl = 0, s_bl = 0, s_att = 0;
  for (const auto& [wl, _] : this->key_wl) s_wl += wl.size() + smart_sizeof(wl.size());
  for (const auto& [bl, _] : this->key_bl) s_bl += bl.size() + smart_sizeof(bl.size());
  for (const auto& [att, _] : this->key_att) s_att += att.size() + smart_sizeof(att.size());

  total_size +=(spol + smart_sizeof(spol)) + (skr + smart_sizeof(skr) + 1) +
               (skwl + smart_sizeof(skwl) + 1) * this->key_wl.size()  + s_wl +
               (skbl + smart_sizeof(skbl) + 1) * this->key_bl.size()  + s_bl +
               (skatt+ smart_sizeof(skatt)+ 1) * this->key_att.size() + s_att +
                sizeof(uint8_t) + sizeof(uint16_t) * 3;

  return total_size;
}

bool KPABE_DPVS_DECRYPTION_KEY::operator==(const KPABE_DPVS_DECRYPTION_KEY &other) const
{
  return this->key_root == other.key_root &&
         map_compare(this->key_wl, other.key_wl) &&
         map_compare(this->key_bl, other.key_bl) &&
         map_compare(this->key_att, other.key_att);
}


bool getSizeFromStream(std::istream &is, size_t *size, ByteString &size_buf) {
  size_buf.fillBuffer(0, hdrLen);
  is.read(reinterpret_cast<char *>(size_buf.getInternalPtr()), static_cast<std::streamsize>(hdrLen));
  if (!is.good()) {
    std::cerr << "Error: Could not read pack size" << std::endl;
    return false;
  }

  size_t index = 1;
  *size = size_buf.get32bits(&index);

  return true;
}

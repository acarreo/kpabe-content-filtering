#include "vector_ec.hpp"


/****************************************************************************/
/*                                G1_VECTOR                                 */
/****************************************************************************/

G1_VECTOR::G1_VECTOR(const g1_vector_ptr & g1_vector) {
  for (size_t i = 0; i < g1_vector->dim; i++) {
    this->push_back(G1(g1_vector->elements[i]));
  }
  this->setDim(g1_vector->dim);
}

void G1_VECTOR::addElement(const G1 & element) {
  if (!this->isDimSet) {
    this->push_back(element);
  }
  else {
    throw std::runtime_error("Cannot add more elements to the vector");
  }
}

void G1_VECTOR::insertElement(const G1 & element, size_t index) {
  if (index < this->size()) {
    this->at(index) = element;
  }
  else {
    throw std::runtime_error("Cannot insert more elements to the vector");
  }
}

size_t G1_VECTOR::getSizeInBytes() const {
  size_t buff_size = 0, total_size = 0;

  if (this->size() == 0) return 0;

  // size of G1_VECTOR in bytes
  buff_size = (BIN_COMPRESSED ? G1_SIZE_BIN_COMPRESSED : G1_SIZE_BIN);
  buff_size *= this->getDim();

  // ADD : type of vector group and size of dim
  total_size = 2 * sizeof(uint8_t);

  // ADD : size of G1_VECTOR in bytes
  total_size += sizeof(uint8_t) + smart_sizeof(buff_size);

  total_size += buff_size; // ADD : buff_size

  // For some reason that I don't know, we need to add 1 to total_size
  return total_size + 1;
}

g1_vector_ptr G1_VECTOR::getG1Vector() const {
  size_t dim = this->getDim();
  g1_vector_ptr g1_vector = nullptr;
  if (dim > 0) {
    g1_vector = (g1_vector_ptr)malloc(sizeof(g1_vector_t));
    g1_vector->elements = (g1_t *)malloc(sizeof(g1_t) * dim);
    g1_vector->dim = dim;

    for (size_t i = 0; i < dim; i++) {
      g1_copy(g1_vector->elements[i], this->at(i).m_G1);
    }
  }
  return g1_vector;
}

void G1_VECTOR::serialize(OpenABEByteString &result) const {
  OpenABEByteString temp;
  uint8_t dim = this->getDim();

  int g1_size = 0;
  for (auto &g1 : *this) {
    uint8_t *g1_bin = g1.getBytes(&g1_size);
    temp.appendArray(g1_bin, g1_size);
  }

  result.clear();
  result.insertFirstByte(VECTOR_G1_ELEMENT);
  result.pack8bits(dim);
  result.smartPack(temp);
}

void G1_VECTOR::deserialize(OpenABEByteString &input) {
  OpenABEByteString temp;
  size_t index = 0, g1_size = G1_SIZE;

  if (input.at(index++) == VECTOR_G1_ELEMENT) {
    uint8_t dim = input.at(index++);
    temp = input.smartUnpack(&index);

    this->clear();
    for (size_t i = 0; i < (size_t)dim; i++) {
      this->push_back(G1(temp.getInternalPtr() + g1_size*i, g1_size));
    }
    this->setDim(dim);
  }
}

bool G1_VECTOR::operator==(const G1_VECTOR &x) const {
  if (this->size() != x.size()) {
    return false;
  }

  for (size_t i = 0; i < this->size(); i++) {
    if (!(this->at(i) == x.at(i))) {
      return false;
    }
  }

  return true;
}

G1_VECTOR & G1_VECTOR::operator=(const G1_VECTOR &other) {
  if (this != &other) {
    static_cast<std::vector<G1>&>(*this) = static_cast<const std::vector<G1>&>(other);
    isDimSet = other.isDimSet;
    dim = other.dim;
  }
  return *this;
}

G1_VECTOR G1_VECTOR::operator+(const G1_VECTOR &other) const {
  if (this->getDim() != other.getDim()) {
    throw std::runtime_error("Cannot add two vectors with different dimensions");
  }

  G1_VECTOR result(this->getDim());
  for (size_t i = 0; i < this->getDim(); i++) {
    result.insertElement(this->at(i) + other.at(i), i);
  }
  return result;
}

G1_VECTOR G1_VECTOR::operator*(const ZP &k) const {
  G1_VECTOR result(this->getDim());
  for (size_t i = 0; i < this->getDim(); i++) {
    result.insertElement(this->at(i) * k, i);
  }
  return result;
}


/****************************************************************************/
/*                                G2_VECTOR                                 */
/****************************************************************************/

G2_VECTOR::G2_VECTOR(const g2_vector_ptr & g2_vector) {
  for (size_t i = 0; i < g2_vector->dim; i++) {
    this->push_back(G2(g2_vector->elements[i]));
  }
  this->setDim(g2_vector->dim);
}

void G2_VECTOR::addElement(const G2 & element) {
  if (this->isDimSet && this->size() < this->dim) {
    this->push_back(element);
  }
  else if (!this->isDimSet) {
    this->push_back(element);
  }
  else {
    throw std::runtime_error("Cannot add more elements to the vector");
  }
}

void G2_VECTOR::insertElement(const G2 & element, size_t index) {
  if (this->isDimSet && index < this->dim) {
    this->at(index) = element;
  }
  else if (index < this->size()) {
    this->at(index) = element;
  }
  else {
    throw std::runtime_error("Cannot insert more elements to the vector");
  }
}

size_t G2_VECTOR::getSizeInBytes() const {
  size_t buff_size = 0, total_size = 0;

  if (this->size() == 0) return 0;

  // size of G2_VECTOR in bytes
  buff_size = G2_SIZE;
  buff_size *= this->getDim();

  // ADD : type of vector group and size of dim
  total_size = 2 * sizeof(uint8_t);

  // ADD : size of G2_VECTOR in bytes
  total_size += sizeof(uint8_t) + smart_sizeof(buff_size);

  total_size += buff_size; // ADD : buff_size

  // For some reason that I don't know, we need to add 1 to total_size
  return total_size + 1;
}

g2_vector_ptr G2_VECTOR::getG2Vector() const {
  size_t dim = this->getDim();
  g2_vector_ptr g2_vector = nullptr;
  if (dim > 0) {
    g2_vector = (g2_vector_ptr)malloc(sizeof(g2_vector_t));
    g2_vector->elements = (g2_t *)malloc(sizeof(g2_t) * dim);
    g2_vector->dim = dim;

    for (size_t i = 0; i < dim; i++) {
      g2_copy(g2_vector->elements[i], this->at(i).m_G2);
    }
  }
  return g2_vector;
}

void G2_VECTOR::serialize(OpenABEByteString &result) const {
  OpenABEByteString temp;
  uint8_t dim = this->getDim();

  int g2_size = 0;
  for (auto &g2 : *this) {
    uint8_t *g2_bin = g2.getBytes(&g2_size);
    temp.appendArray(g2_bin, g2_size);
  }

  result.clear();
  result.insertFirstByte(VECTOR_G2_ELEMENT);
  result.pack8bits(dim);
  result.smartPack(temp);
}

void G2_VECTOR::deserialize(OpenABEByteString &input) {
  OpenABEByteString temp;
  size_t index = 0, g2_size = G2_SIZE;

  if (input.at(index++) == VECTOR_G2_ELEMENT) {
    uint8_t dim = input.at(index++);
    temp = input.smartUnpack(&index);

    this->clear();
    for (size_t i = 0; i < (size_t)dim; i++) {
      this->push_back(G2(temp.getInternalPtr() + g2_size*i, g2_size));
    }
    this->setDim(dim);
  }
}

bool G2_VECTOR::operator==(const G2_VECTOR &x) const {
  if (this->size() != x.size()) {
    return false;
  }

  for (size_t i = 0; i < this->size(); i++) {
    if (!(this->at(i) == x.at(i))) {
      return false;
    }
  }

  return true;
}

G2_VECTOR & G2_VECTOR::operator=(const G2_VECTOR &other) {
  if (this != &other) {
    static_cast<std::vector<G2>&>(*this) = static_cast<const std::vector<G2>&>(other);
    isDimSet = other.isDimSet;
    dim = other.dim;
  }
  return *this;
}

G2_VECTOR G2_VECTOR::operator+(const G2_VECTOR &other) const {
  if (this->getDim() != other.getDim()) {
    throw std::runtime_error("Cannot add two vectors with different dimensions");
  }

  G2_VECTOR result(this->getDim());
  for (size_t i = 0; i < this->getDim(); i++) {
    result.insertElement(this->at(i) + other.at(i), i);
  }
  return result;
}

G2_VECTOR G2_VECTOR::operator*(const ZP &k) const {
  G2_VECTOR result(this->getDim());
  for (size_t i = 0; i < this->getDim(); i++) {
    result.insertElement(this->at(i) * k, i);
  }
  return result;
}


GT innerProduct(const G1_VECTOR &x, const G2_VECTOR &y) {
  if (x.getDim() != y.getDim()) {
    throw std::runtime_error("Cannot compute inner product of two vectors with different dimensions");
  }

  GT result;

  g1_vector_ptr vx = x.getG1Vector();
  g2_vector_ptr vy = y.getG2Vector();

  if (vx != nullptr && vy != nullptr) {
    pp_map_sim_k12(result.m_GT, vx->elements, vy->elements, vx->dim);
  }

  clear_g1_vector(vx);
  clear_g2_vector(vy);

  return result;
}

void clear_g1_vector(g1_vector_ptr &g1_vector) {
  if (g1_vector != nullptr) {
    for (size_t i = 0; i < g1_vector->dim; i++) {
      g1_free(g1_vector->elements[i]);
    }
    free(g1_vector->elements);
    free(g1_vector);
    g1_vector = nullptr;
  }
}

void clear_g2_vector(g2_vector_ptr &g2_vector) {
  if (g2_vector != nullptr) {
    for (size_t i = 0; i < g2_vector->dim; i++) {
      g2_free(g2_vector->elements[i]);
    }
    free(g2_vector->elements);
    free(g2_vector);
    g2_vector = nullptr;
  }
}

ZP hashToZP(const std::string &str) {
  ZP result;
  uint8_t hash[RLC_MD_LEN];
  md_map(hash, (uint8_t*)str.c_str(), str.length());
  bn_read_bin(result.m_ZP, hash, RLC_MD_LEN);
  return result;
}

ZP hashToZP(const std::string &str, const bn_t order) {
  ZP result;
  uint8_t hash[RLC_MD_LEN];
  md_map(hash, (uint8_t*)str.c_str(), str.length());
  bn_read_bin(result.m_ZP, hash, RLC_MD_LEN);
  bn_mod(result.m_ZP, result.m_ZP, order);
  result.setOrder(order);
  return result;
}

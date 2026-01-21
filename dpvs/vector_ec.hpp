#ifndef __VECTOR_EC_H__
#define __VECTOR_EC_H__

#include <vector>
#include <abe_lsss/abe_lsss.h>

extern "C" {
  #include "dpvs.h"
}

#define smart_sizeof(x)   ((((x) > UINT16_MAX) ? sizeof(uint32_t) : (((x) > UINT8_MAX) ? sizeof(uint16_t) : sizeof(uint8_t))))

// Can be defined in the CMakelists.txt
#ifndef _COMPRESSION_
#define _COMPRESSION_     true
#endif

#define BIN_COMPRESSED    _COMPRESSION_

typedef enum ElementType {
  VECTOR_G1_ELEMENT = 0xF1,
  VECTOR_G2_ELEMENT = 0xF2,
} ElementType;

using ByteString = OpenABEByteString;

class G1_VECTOR : public std::vector<G1>, public ZObject {
private:
  size_t dim;
  bool isDimSet;

public:
  G1_VECTOR() : std::vector<G1>(), ZObject(), isDimSet(false) {}
  G1_VECTOR(size_t dim) : std::vector<G1>(dim), ZObject(), dim(dim), isDimSet(true) {}
  G1_VECTOR(std::initializer_list<G1> init_list) : std::vector<G1>(init_list), ZObject(), isDimSet(false) {}
  G1_VECTOR(const G1_VECTOR &other) : std::vector<G1>(other), ZObject(), dim(other.dim), isDimSet(other.isDimSet) {}
  G1_VECTOR(const g1_vector_ptr &g1_vector);

  ~G1_VECTOR() { this->clear(); this->dim = 0; this->isDimSet = false; }

  void setDim(size_t dim) {
    if (!this->isDimSet) {
      this->resize(dim); this->dim = dim; this->isDimSet = true;
    }
  }

  size_t getDim() const { return this->isDimSet ? this->dim : this->size(); }

  size_t getSizeInBytes() const;

  g1_vector_ptr getG1Vector() const;

  bool isFixedSize() const { return this->isDimSet; }

  void addElement(const G1 &element);
  void insertElement(const G1 &element, size_t index);

  void serialize(ByteString &result) const;
  void deserialize(ByteString &input);

  bool operator==(const G1_VECTOR &x) const;
  G1_VECTOR& operator=(const G1_VECTOR &other);
  G1_VECTOR  operator+(const G1_VECTOR &other) const;
  G1_VECTOR  operator*(const ZP &k) const;

  // Temporary methods for testing, will be removed later
  void random(size_t dim) {
    G1_VECTOR vect(dim);
    for (size_t i = 0; i < dim; i++) {
      G1 g1; g1.setRandom();
      vect.insertElement(g1, i);
    }
    *this = vect;
  }

};


class G2_VECTOR : public std::vector<G2> {
private:
  size_t dim;
  bool isDimSet;

public:
  G2_VECTOR() : std::vector<G2>(), isDimSet(false) {}
  G2_VECTOR(size_t dim) : std::vector<G2>(dim), dim(dim), isDimSet(true) {}
  G2_VECTOR(std::initializer_list<G2> init_list) : std::vector<G2>(init_list), isDimSet(false) {}
  G2_VECTOR(const G2_VECTOR &other) : std::vector<G2>(other), dim(other.dim), isDimSet(other.isDimSet) {}
  G2_VECTOR(const g2_vector_ptr &g2_vector);

  ~G2_VECTOR() {
    this->clear(); this->dim = 0; this->isDimSet = false;
  }

  void setDim(size_t dim) {
    if (!this->isDimSet) {
      this->resize(dim); this->dim = dim; this->isDimSet = true;
    }
  }

  size_t getDim() const {
    return this->isDimSet ? this->dim : this->size();
  }

  size_t getSizeInBytes() const;

  g2_vector_ptr getG2Vector() const;

  bool isFixedSize() const { return this->isDimSet; }

  void addElement(const G2 &element);
  void insertElement(const G2 &element, size_t index);

  void serialize(ByteString &result) const;
  void deserialize(ByteString &input);

  bool operator==(const G2_VECTOR &x) const;
  G2_VECTOR& operator=(const G2_VECTOR &other);
  G2_VECTOR  operator+(const G2_VECTOR &other) const;
  G2_VECTOR  operator*(const ZP &k) const;

  // Temporary methods for testing, will be removed later
  void random(size_t dim) {
    G2 g2;
    G2_VECTOR vect;
    for (size_t i = 0; i < dim; i++) {
      g2.setRandom();
      vect.push_back(g2);
    }
    vect.setDim(dim);
    *this = vect;
  }

};


// Inner product of two vectors
GT innerProduct(const G1_VECTOR &x, const G2_VECTOR &y);

void clear_g1_vector(g1_vector_ptr &g1_vector);
void clear_g2_vector(g2_vector_ptr &g2_vector);

template <typename Map>
bool map_compare (Map const &lhs, Map const &rhs) {
  return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

ZP hashToZP(const std::string &str);
ZP hashToZP(const std::string &str, const bn_t order);

#endif // __VECTOR_EC_H__

/**
 * @file dpvs_advanced.cpp
 * @author Adam Oumar Abdel-rahman
 * @brief Implementation of the advanced DPVS vectors classes
 * @date 2023-08-09
 *
 */


#include "dpvs_advanced.hpp"


// Temporary function to generate random vectors, for testing purposes

void random_vect_g1 (G1_VS_VECT vect) {
  if (vect && vect->coord) {
    for (uint8_t i = 0; i < vect->dim; i++) {
      g1_rand(vect->coord[i]);
    }
  }
}

void random_vect_g2 (G2_VS_VECT vect) {
  if (vect && vect->coord) {
    for (uint8_t i = 0; i < vect->dim; i++) {
      g2_rand(vect->coord[i]);
    }
  }
}


/****************************************************************************/
/*                                G1_VECTOR                                 */
/****************************************************************************/

G1_VECTOR::G1_VECTOR(const G1_VS_VECT& vector) {
  if (vector && vector->coord) {
    this->dim = vector->dim;
    this->vect = dpvs_create_g1_vect(this->dim);
    dpvs_copy_g1_vect(this->vect, vector);
  }
}

G1_VECTOR::G1_VECTOR(const G1_VECTOR& other) {
  if (other.vect) {
    this->dim = other.dim;
    this->vect = dpvs_create_g1_vect(this->dim);
    dpvs_copy_g1_vect(this->vect, other.vect);
  }
}

/**
 * @brief Get the g1 vect object
 * 
 * @param result_vect The result vector
 */
void G1_VECTOR::get_g1_vect(G1_VS_VECT& result_vect) {
  if (!result_vect || this->dim != result_vect->dim) {
    dpvs_clear_g1_vect(result_vect);
    result_vect = dpvs_create_g1_vect(this->dim);
  }
  dpvs_copy_g1_vect(result_vect, this->vect);
}

bool G1_VECTOR::operator==(const G1_VECTOR& other) const {
  return dpvs_compare_g1_vect(this->vect, other.vect);
}

G1_VECTOR& G1_VECTOR::operator=(const G1_VECTOR& other) {
  if (this != &other) {
    G1_VECTOR temp(other);
    std::swap(this->vect, temp.vect);
    std::swap(this->dim, temp.dim);
  }
  return *this;
}

G1_VECTOR G1_VECTOR::operator+(const G1_VECTOR& other) const {
  G1_VECTOR result(this->dim);
  dpvs_add_g1_vect(result.vect, this->vect, other.vect);
  return result;
}

G1_VECTOR G1_VECTOR::operator*(const bn_t k) const {
  G1_VECTOR result(this->dim);
  dpvs_k_mul_g1_vect(result.vect, this->vect, k);
  return result;
}

/**
 * @brief Serialize the G1 vector to the output stream
 * 
 * @param os The output stream
 */
void G1_VECTOR::serialize(std::ostream &os) const {
  GROUP_VECTOR_TYPE type = this->vect_type;
  uint8_t* buffer = nullptr;
  int buffer_size = 0;
  if (!serialize_g1_vector(this->vect, &buffer, &buffer_size)) {
    os.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
    os.write(reinterpret_cast<const char*>(&buffer_size), sizeof(buffer_size));
    os.write(reinterpret_cast<const char*>(buffer), buffer_size);
    free(buffer);
  }
}

/**
 * @brief Deserialize the G1 vector from the input stream
 * 
 * @param is The input stream, the vector type should be read before calling this function
 * @return G1_VECTOR* The deserialized G1 vector, nullptr if an error occured
 */
G1_VECTOR* deserialize_g1_vect(std::istream &is) {
  G1_VS_VECT g1_vect = nullptr;
  G1_VECTOR* result;

  int buffer_size = 0;
  is.read(reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
  if (is && buffer_size > 0) {
    uint8_t* buffer = new uint8_t[buffer_size];
    is.read(reinterpret_cast<char*>(buffer), buffer_size);
    if (is && !deserialize_g1_vector(buffer, buffer_size, &g1_vect)) {
      result = new G1_VECTOR(g1_vect);
      dpvs_clear_g1_vect(g1_vect);
      delete [] buffer;
    }
  }

  return result;
}


/****************************************************************************/
/*                                G2_VECTOR                                 */
/****************************************************************************/

G2_VECTOR::G2_VECTOR(const G2_VS_VECT &vector) {
  if (vector && vector->coord) {
    this->dim = vector->dim;
    this->vect = dpvs_create_g2_vect(vector->dim);
    dpvs_copy_g2_vect(this->vect, vector);
  }
}

G2_VECTOR::G2_VECTOR(const G2_VECTOR& other) {
  if (other.vect) {
    this->dim = other.dim;
    this->vect = dpvs_create_g2_vect(this->dim);
    dpvs_copy_g2_vect(this->vect, other.vect);
  }
}


/**
 * @brief Get the g2 vect object
 * 
 * @param result_vect The result vector
 */
void G2_VECTOR::get_g2_vect(G2_VS_VECT& result_vect) {
  if (!result_vect || this->dim != result_vect->dim) {
    dpvs_clear_g2_vect(result_vect);
    result_vect = dpvs_create_g2_vect(this->dim);
  }
  dpvs_copy_g2_vect(result_vect, this->vect);
}

bool G2_VECTOR::operator==(const G2_VECTOR& other) const {
  return dpvs_compare_g2_vect(this->vect, other.vect);
}

G2_VECTOR& G2_VECTOR::operator=(const G2_VECTOR& other) {
  if (this != &other) {
    G2_VECTOR temp(other);
    std::swap(this->vect, temp.vect);
    std::swap(this->dim, temp.dim);
  }
  return *this;
}

G2_VECTOR G2_VECTOR::operator+(const G2_VECTOR& other) const {
  G2_VECTOR result(this->dim);
  dpvs_add_g2_vect(result.vect, this->vect, other.vect);
  return result;
}

G2_VECTOR G2_VECTOR::operator*(const bn_t k) const {
  G2_VECTOR result(this->dim);
  dpvs_k_mul_g2_vect(result.vect, this->vect, k);
  return result;
}

/**
 * @brief Serialize the G2 vector to the output stream
 * 
 * @param os The output stream
 */
void G2_VECTOR::serialize(std::ostream &os) const {
  GROUP_VECTOR_TYPE type = this->vect_type;
  uint8_t* buffer = nullptr;
  int buffer_size = 0;
  if (!serialize_g2_vector(this->vect, &buffer, &buffer_size)) {
    os.write(reinterpret_cast<const char*>(&type), sizeof(uint8_t));
    os.write(reinterpret_cast<const char*>(&buffer_size), sizeof(buffer_size));
    os.write(reinterpret_cast<const char*>(buffer), buffer_size);
    free(buffer);
  }
}

/**
 * @brief Deserialize the G2 vector from the input stream
 * 
 * @param is The input stream, the vector type should be read before calling this function
 * @return G2_VECTOR* The deserialized G2 vector, nullptr if failed
 */
G2_VECTOR* deserialize_g2_vect(std::istream &is) {
  G2_VS_VECT g2_vect = nullptr;
  G2_VECTOR* result;

  int buffer_size = 0;
  is.read(reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
  if (is && buffer_size > 0) {
    uint8_t* buffer = new uint8_t[buffer_size];
    is.read(reinterpret_cast<char*>(buffer), buffer_size);
    if (is && !deserialize_g2_vector(buffer, buffer_size, &g2_vect)) {
      result = new G2_VECTOR(g2_vect);
      dpvs_clear_g2_vect(g2_vect);
      delete [] buffer;
    }
  }

  return result;
}


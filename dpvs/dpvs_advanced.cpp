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
void G1_VECTOR::get_g1_vect(G1_VS_VECT& result_vect) const {
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
 * @param[in] os The output stream
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
 * @param[in] is The input stream
 */
void G1_VECTOR::deserialize(std::istream &is) {
  int buffer_size = 0;
  G1_VS_VECT vs_vect = nullptr;
  uint8_t type = is.get();
  is.read(reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
  if (type == G1_VECT && buffer_size > 0 && is) {
    uint8_t* buffer = new uint8_t[buffer_size];
    is.read(reinterpret_cast<char*>(buffer), buffer_size);
    if (is && !deserialize_g1_vector(buffer, buffer_size, &vs_vect)) {
      // Clear the current vector
      dpvs_clear_g1_vect(this->vect);

      this->dim = vs_vect->dim;
      this->vect = vs_vect;

      delete [] buffer;
    }
  }
}

/**
 * @brief Get the size of the vector in bytes
 *
 * @return int The size of the vector in bytes, including the type and the size of the buffer.
 *         If the vector is empty, or an error occurs, the size is 0.
 */
int G1_VECTOR::bytes_size() const
{
  int size = 0;
  if (!sizeof_g1_vector(this->vect, &size))
    size += sizeof(uint8_t) + sizeof(int);

  return size;
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
void G2_VECTOR::get_g2_vect(G2_VS_VECT& result_vect) const {
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
 * @param[in] os The output stream
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
 * @param[in] is The input stream
 */
void G2_VECTOR::deserialize(std::istream &is) {
  int buffer_size = 0;
  G2_VS_VECT vs_vect = nullptr;
  uint8_t type = is.get();
  is.read(reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
  if (type == G2_VECT && buffer_size > 0 && is) {
    uint8_t* buffer = new uint8_t[buffer_size];
    is.read(reinterpret_cast<char*>(buffer), buffer_size);
    if (is && !deserialize_g2_vector(buffer, buffer_size, &vs_vect)) {
      // Clear the current vector
      dpvs_clear_g2_vect(this->vect);

      this->dim = vs_vect->dim;
      this->vect = vs_vect;

      delete [] buffer;
    }
  }
}

/**
 * @brief Get the size of the vector in bytes
 *
 * @return int The size of the vector in bytes, including the type and the size of the buffer.
 *         If the vector is empty, or an error occurs, the size is 0.
 */
int G2_VECTOR::bytes_size() const
{
  int size = 0;
  if (!sizeof_g2_vector(this->vect, &size))
    size += sizeof(uint8_t) + sizeof(int);

  return size;
}

void inner_product(gt_t result, const G1_VECTOR &vect1, const G2_VECTOR &vect2)
{
  G1_VS_VECT vs_vect1 = nullptr; vect1.get_g1_vect(vs_vect1);
  G2_VS_VECT vs_vect2 = nullptr; vect2.get_g2_vect(vs_vect2);

  dpvs_inner_product(result, vs_vect1, vs_vect2);

  dpvs_clear_g1_vect(vs_vect1);
  dpvs_clear_g2_vect(vs_vect2);
}

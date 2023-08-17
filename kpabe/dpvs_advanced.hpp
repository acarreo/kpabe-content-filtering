/**
 * @file dpvs_advanced.hpp
 * @author Adam Oumar Abdel-rahman
 * @brief Definition of the advanced DPVS vectors classes
 * @date 2023-08-09
 *
 */

#ifndef __DPVS_HPP__
#define __DPVS_HPP__

#include <iostream>

extern "C" {
  #include "../serial/serial.h"
  #include "../dpvs/dpvs.h"
  #include "../utils.h"
}

typedef enum {
  G1_VECT = 1,
  G2_VECT
} GROUP_VECTOR_TYPE;

void random_vect_g1 (G1_VS_VECT vect);
void random_vect_g2 (G2_VS_VECT vect);


/**
 * @brief Class for G1 vectors, i.e. vectors of G1 elements
 *
 */
class G1_VECTOR {
  public:
    G1_VECTOR() : vect(nullptr), dim(0) {}
    G1_VECTOR(uint8_t dim) { this->vect = dpvs_create_g1_vect(dim); this->dim = dim; }
    G1_VECTOR(const G1_VS_VECT& vector);
    G1_VECTOR(const G1_VECTOR& other);
    ~G1_VECTOR() { dpvs_clear_g1_vect(this->vect); }

    void get_g1_vect(G1_VS_VECT& result_vect) const;

    bool operator==(const G1_VECTOR& other) const;
    G1_VECTOR& operator=(const G1_VECTOR& other);
    G1_VECTOR operator+(const G1_VECTOR& other) const;
    G1_VECTOR operator*(const bn_t k) const; // scalar multiplication

    void serialize(std::ostream& os) const;

    static const GROUP_VECTOR_TYPE vect_type = G1_VECT;

    // Temporary function for testing
    void random_vector() { random_vect_g1(this->vect); }
    void print_vector() { print_vect_base(this->vect); }

  private:
    G1_VS_VECT vect;
    uint8_t dim;
};


/**
 * @brief Class for G2 vectors, i.e. vectors of G2 elements
 *
 */
class G2_VECTOR {
  public:
    G2_VECTOR() : vect(nullptr), dim(0) {}
    G2_VECTOR(uint8_t dim) { this->vect = dpvs_create_g2_vect(dim); this->dim = dim; }
    G2_VECTOR(const G2_VS_VECT& vector);
    G2_VECTOR(const G2_VECTOR& other);
    ~G2_VECTOR() { dpvs_clear_g2_vect(this->vect); }

    void get_g2_vect(G2_VS_VECT& result_vect) const;

    bool operator==(const G2_VECTOR& other) const;
    G2_VECTOR& operator=(const G2_VECTOR& other);
    G2_VECTOR operator+(const G2_VECTOR& other) const;
    G2_VECTOR operator*(const bn_t k) const; // scalar multiplication

    void serialize(std::ostream& os) const;

    static const GROUP_VECTOR_TYPE vect_type = G2_VECT;

    // Temporary function for testing
    void random_vector() { random_vect_g2(this->vect); }
    void print_vector() { print_vect_dual_base(this->vect); }

  private:
    G2_VS_VECT vect;
    uint8_t dim;
};


G1_VECTOR* deserialize_g1_vect(std::istream& is);
G2_VECTOR* deserialize_g2_vect(std::istream& is);


#endif // __DPVS_ADVANCED_HPP__

#ifndef __DPVS_H__
#define __DPVS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "matrix.h"

typedef struct
{
  uint8_t dim;  // Dimension of the vector
  g1_t* elements;  // Coordinates of vector of G1 elements
} g1_vector_t, *g1_vector_ptr;

typedef struct
{
  uint8_t dim;  // Dimension of the vector
  g2_t* elements;  // Coordinates of the vector of G2 elements
} g2_vector_t, *g2_vector_ptr;

typedef struct
{
  uint8_t dim;           // Dimension of the base
  g1_vector_ptr *base;      // Base of G1^dim vector space
  g2_vector_ptr *dual_base; // Base of G2^dim (dual space of G1^dim)
} dpvs_t;

#define G1_VS_BASE g1_vector_ptr* // Base of G1^dim vector space
#define G2_VS_BASE g2_vector_ptr* // Base of G2^dim (dual space of G1^dim)

/* Initialize a base implies initialisation matrix */
#define dpvs_get_mat_row    mat_get_row
#define dpvs_gen_matrices   mat_rand_dual_mat

#ifdef __cplusplus
}
#endif

g1_vector_ptr dpvs_create_g1_vect(uint8_t dim);
g2_vector_ptr dpvs_create_g2_vect(uint8_t dim);
G1_VS_BASE dpvs_create_g1_base(uint8_t dim);
G2_VS_BASE dpvs_create_g2_base(uint8_t dim);

dpvs_t* dpvs_create_bases(uint8_t dim);
dpvs_t* dpvs_generate_bases(uint8_t dim);

/* Compute the scalar multiplication of a vector */
void dpvs_k_mul_g1_vect(g1_vector_ptr dest, const g1_vector_ptr src, const bn_t k);
void dpvs_k_mul_g2_vect(g2_vector_ptr dest, const g2_vector_ptr src, const bn_t k);

void dpvs_add_g1_vect(g1_vector_ptr dest, const g1_vector_ptr src1, const g1_vector_ptr src2);
void dpvs_add_g2_vect(g2_vector_ptr dest, const g2_vector_ptr src1, const g2_vector_ptr src2);

void dpvs_copy_g1_vect (g1_vector_ptr dest, g1_vector_ptr src);
void dpvs_copy_g2_vect (g2_vector_ptr dest, g2_vector_ptr src);

bool dpvs_compare_g1_vect(const g1_vector_ptr vect1, const g1_vector_ptr vect2);
bool dpvs_compare_g2_vect(const g2_vector_ptr vect1, const g2_vector_ptr vect2);

static __inline__ void dpvs_inner_product(gt_t ip, const g1_vector_ptr vect, g2_vector_ptr dvect)
{
  if (vect->dim == dvect->dim)
    pp_map_sim_oatep_k12(ip, vect->elements, dvect->elements, vect->dim);
}

void dpvs_clear_g1_vect(g1_vector_ptr bvect);
void dpvs_clear_g2_vect(g2_vector_ptr dbvect);
void dpvs_clear(dpvs_t* dpvs);

#endif

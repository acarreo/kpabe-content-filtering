#ifndef __DPVS_H__
#define __DPVS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "matrix.h"

typedef struct
{
  uint8_t dim;  // Dimension of the vector
  g1_t* coord;  // Coordinates of vector of G1 elements
} g1_vect_t, *G1_VS_VECT;

typedef struct
{
  uint8_t dim;  // Dimension of the vector
  g2_t* coord;  // Coordinates of the vector of G2 elements
} g2_vect_t, *G2_VS_VECT;

typedef struct
{
  uint8_t dim;           // Dimension of the base
  G1_VS_VECT *base;      // Base of G1^dim vector space
  G2_VS_VECT *dual_base; // Base of G2^dim (dual space of G1^dim)
} dpvs_t;

#define G1_VS_BASE G1_VS_VECT* // Base of G1^dim vector space
#define G2_VS_BASE G2_VS_VECT* // Base of G2^dim (dual space of G1^dim)

/* Initialize a base implies initialisation matrix */
#define dpvs_get_mat_row    mat_get_row
#define dpvs_gen_matrices   mat_rand_dual_mat

#ifdef __cplusplus
}
#endif

G1_VS_VECT dpvs_create_g1_vect(uint8_t dim);
G2_VS_VECT dpvs_create_g2_vect(uint8_t dim);
G1_VS_BASE dpvs_create_g1_base(uint8_t dim);
G2_VS_BASE dpvs_create_g2_base(uint8_t dim);

dpvs_t* dpvs_create_bases(uint8_t dim);
dpvs_t* dpvs_generate_bases(uint8_t dim);

/* Compute the scalar multiplication of a vector */
void dpvs_k_mul_g1_vect(G1_VS_VECT dest, const G1_VS_VECT src, const bn_t k);
void dpvs_k_mul_g2_vect(G2_VS_VECT dest, const G2_VS_VECT src, const bn_t k);

void dpvs_add_g1_vect(G1_VS_VECT dest, const G1_VS_VECT src1, const G1_VS_VECT src2);
void dpvs_add_g2_vect(G2_VS_VECT dest, const G2_VS_VECT src1, const G2_VS_VECT src2);

void dpvs_copy_g1_vect (G1_VS_VECT dest, G1_VS_VECT src);
void dpvs_copy_g2_vect (G2_VS_VECT dest, G2_VS_VECT src);

bool dpvs_compare_g1_vect(const G1_VS_VECT vect1, const G1_VS_VECT vect2);
bool dpvs_compare_g2_vect(const G2_VS_VECT vect1, const G2_VS_VECT vect2);

static __inline__ void dpvs_inner_product(gt_t ip, const G1_VS_VECT vect, G2_VS_VECT dvect)
{
  if (vect->dim == dvect->dim)
    pp_map_sim_oatep_k12(ip, vect->coord, dvect->coord, vect->dim);
}

void dpvs_clear_g1_vect(G1_VS_VECT bvect);
void dpvs_clear_g2_vect(G2_VS_VECT dbvect);
void dpvs_clear(dpvs_t* dpvs);

void set_params(void);
void clear_params(void);

#endif

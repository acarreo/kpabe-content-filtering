#ifndef _DPVS_
#define _DPVS_

#include "../matrix/matrix.h"

typedef struct
{
  gt_t gt;
  bn_t q;
} group_setting_st;

typedef group_setting_st group_setting_t[1];

typedef struct
{
  uint8_t dim;
  g1_t* coord;
} g1_vect_st;

typedef g1_vect_st g1_vect_t[1];

typedef struct
{
  uint8_t dim;
  g2_t* coord;
} g2_vect_st;

typedef g2_vect_st g2_vect_t[1];

typedef struct
{
  uint8_t dim;
  g1_vect_st **base;
  g2_vect_st **dual_base;
} dpvs_st;

typedef dpvs_st dpvs_t[1];

extern group_setting_t params;

/* Initialize a base implies initialisation matrix */
#define dpvs_get_mat_row    mat_get_row
#define dpvs_gen_matrices   mat_rand_dual_mat


bool dpvs_init_base_vect(g1_vect_t bvect, uint8_t dim);
bool dpvs_init_dual_base_vect(g2_vect_t db_vect, uint8_t dim);
bool dpvs_init(dpvs_t dpvs, uint8_t dim);

bool dpvs_gen(dpvs_t dpvs, uint8_t dim);

/* Compute the scalar multiplication of a vector */
void dpvs_k_mul_vect(g1_vect_t dest, const g1_vect_t src, bn_t k);
void dpvs_k_mul_dual_vect(g2_vect_t dest, const g2_vect_t src, bn_t k);

void dpvs_inner_product(gt_t ip, const g1_vect_t vect, g2_vect_t dvect);

void dpvs_clear_base_vect(g1_vect_t bvect);
void dpvs_clear_dual_base_vect(g2_vect_t dbvect);
void dpvs_clear(dpvs_t dpvs);

void set_params(void);
void clear_params(void);

#endif
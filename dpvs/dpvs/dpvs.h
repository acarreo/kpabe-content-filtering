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

typedef vect_st vect_t[1];
typedef mat_st  base_t[1];  /* Une base peut être vue comme une matrice */

/* Initialize a base implies initialisation matrix */
#define base_init       mat_init
#define dpvs_gen_matrices   mat_rand_dual_mat


void get_vect_from_base(vect_t vect, const base_t base, uint8_t index);
bool dpvs_init_base_vect(g1_vect_t bvect, uint8_t dim);
bool dpvs_init_dual_base_vect(g2_vect_t db_vect, uint8_t dim);
bool dpvs_init(dpvs_t dpvs, uint8_t dim);


void dpvs_clear_base_vect(g1_vect_t bvect);
void dpvs_clear_dual_base_vect(g2_vect_t dbvect);
void dpvs_clear(dpvs_t dpvs);


#endif
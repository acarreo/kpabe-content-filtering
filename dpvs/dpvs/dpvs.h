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

typedef vect_st vect_t[1];
typedef mat_st  base_t[1];  /* Une base peut être vue comme une matrice */

/* Initialize a base implies initialisation matrix */
#define base_init       mat_init

/* generate radom PDOP (Pair of Dual Orthogonal Bases)
 * bool base_rand_pdop(base_t base, base_t dual_base);
 */
#define base_rand_pdop  mat_rand_inv

bool vect_init(vect_t vect, uint8_t dim);
void get_vect_from_base(vect_t vect, const base_t base, uint8_t index);


#endif
#ifndef _BASE_
#define _BASE_

#include <relic.h>
#include <inttypes.h>
#include <stdbool.h>

#include "matrix.h"

typedef struct
{
    bn_t* coord;
    uint8_t dim;
} vect_st;

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
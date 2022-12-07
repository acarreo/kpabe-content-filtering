#ifndef _MATRICES_
#define _MATRICES_

#include <relic.h>
#include <inttypes.h>
#include <stdbool.h>

#define MAT_INLINE          static __inline__
#define mat_inv             invert_matrix

#define MAT(mat, i, j)          mat->entries[((i) * (mat->dim) + (j))]
#define bn_mod_mul(c, a, b, m)  ({bn_mul(c, a, b); bn_mod(c, c, m);})
#define bn_mod_add(c, a, b, m)  ({bn_add(c, a, b); bn_mod(c, c, m);})
#define bn_mod_sub(c, a, b, m)  ({bn_sub(c, a, b); bn_mod(c, c, m);})
#define mat_dim(mat)            (mat->dim)
#define mat_is_empty(mat)       (((mat->dim) == 0))

#define mat_print(mat)          mat_fprint(stdout, 16, mat)

extern bn_t Fq; /* Declaration of the modulus */

//#define RLC_FQ_BITS         

typedef struct
{
    bn_t * entries;
    uint8_t dim;
} mat_st;

typedef mat_st mat_t[1];

bool mat_init(mat_t mat, uint8_t dim);
bool mat_cmp(const mat_t A, const mat_t B);
bool mat_is_zero(const mat_t mat);
void mat_eye(mat_t mat, uint8_t dim);
void mat_transpose(mat_t dest, const mat_t src);
void mat_copy(mat_t dest, const mat_t src);
void mat_rand(mat_t mat);
void mat_product(mat_t dest, const mat_t A, const mat_t B);
void mat_rand_inv(mat_t A, mat_t inv_A);
bool invert_matrix(mat_t dest, const mat_t src);

void mat_clear(mat_t mat);

int mat_fprint(FILE * file, int format, const mat_t mat);
int mat_fread(FILE* file, int format, mat_t mat);

#endif

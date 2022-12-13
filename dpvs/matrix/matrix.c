#include "matrices.h"


bn_t Fq;    /* Definition of modulus, it's declared in matrices.h */

MAT_INLINE bool _error_mat_alloc_(void)
{
    fprintf(stderr, "ALLOC matrix failled\n");
    return false;
}

bool bn_vect_init(bn_vect_t vect, uint8_t dim)
bool mat_init(mat_t mat, uint8_t dim)
{
  vect->dim = dim;

  if (dim != 0)
  {
    if ((vect->coord = malloc(dim * sizeof(bn_t))) == NULL)
      return _error_alloc_fail_();

    int i = 0;
    RLC_TRY {
      for (; i < dim; i++)
      {
        bn_null(vect->coord[i]);
        bn_new(vect->coord[i]);
      }
    } RLC_CATCH_ANY {
      vect->dim = i;
      bn_vect_clear(vect);
      return _error_alloc_fail_();
    mat->dim = dim;

    int mat_size = dim * dim;
    if (dim != 0)
    {
        if ((mat->entries = malloc(mat_size * sizeof(bn_t))) == NULL)
            return _error_mat_alloc_();

        int i = 0;
        RLC_TRY {
            for (; i < mat_size; i++)
            {
                bn_null(mat->entries[i]);
                bn_new(mat->entries[i]);
            }
        } RLC_CATCH_ANY {
            for (; i > 0; i--) bn_free(mat->entries[i-1]);
            return _error_mat_alloc_();
        }
    }
  }

  return true;
}

bool mat_cmp(const mat_t A, const mat_t B)
{
    bool test_cmp = false;

    if (mat_dim(A) == mat_dim(B))
    {
        test_cmp = true;
        for (int i = 0; i < mat_dim(A) * mat_dim(A) && test_cmp; i++)
            test_cmp = (bn_cmp(A->entries[i], B->entries[i]) == RLC_EQ);
    }

    return test_cmp;
}

bool mat_is_zero(const mat_t mat)
{
    bool is_zero = true;

    for (int i = 0; i < mat_dim(mat) * mat_dim(mat) && is_zero; i++)
        is_zero = bn_is_zero(mat->entries[i]);

    return is_zero;
}

void mat_eye(mat_t mat, uint8_t dim)
{
    mat_clear(mat);
    if (mat_init(mat, dim))
    {
        bn_t one;

        bn_null(one);
        bn_new(one);
        bn_set_dig(one, 1);

        for (uint8_t i = 0; i < mat_dim(mat); i++)
            bn_copy(MAT(mat, i, i), one);

        bn_free(one);
    }
}

void mat_transpose(mat_t dest, const mat_t src)
{
    if (!mat_is_empty(src))
    {
        mat_clear(dest);
        mat_init(dest, mat_dim(src));

        for (uint8_t i = 0; i < mat_dim(src); i++)
            for (uint8_t j = 0; j < mat_dim(src); j++)
                bn_copy(MAT(dest, i, j), MAT(src, j, i));
    }
}

void mat_copy(mat_t dest, const mat_t src)
{
    mat_clear(dest);

    if (mat_init(dest, mat_dim(src)))
        for (int i = 0; i < mat_dim(src) * mat_dim(src); i++)
            bn_copy(dest->entries[i], src->entries[i]);
}

void mat_rand(mat_t mat)
{
    for (int i = 0; i < mat_dim(mat) * mat_dim(mat); i++)
        bn_rand_mod(mat->entries[i], Fq);
}

void mat_product(mat_t dest, const mat_t A, const mat_t B)
{
    uint8_t dim = mat_dim(A);

    if (dim != 0 && dim == mat_dim(B))
    {
        bn_t tmp, sum;

        bn_null(tmp);
        bn_null(sum);

        RLC_TRY {
            bn_new(tmp);
            bn_new(sum);
            for (uint8_t i = 0; i < dim; i++)
            {
                for (uint8_t j = 0; j < dim; j++)
                {
                    bn_zero(sum);
                    for (uint8_t k = 0; k < dim; k++) {
                        bn_mod_mul(tmp, MAT(A, i, k), MAT(B, k, j), Fq);
                        bn_mod_add(sum, sum, tmp, Fq);
                    }
                    bn_copy(MAT(dest, i, j), sum);
                }
            }
        } RLC_CATCH_ANY {
            fprintf(stderr, "Something goes wrong\n");
        } RLC_FINALLY {
            bn_free(tmp);
            bn_free(sum);
        }
    }
}

void mat_rand_inv(mat_t A, mat_t inv_A)
{
    mat_rand(A);
    while (!mat_inv(inv_A, A)) mat_rand(A);
}

int mat_fprint(FILE * file, int format, const mat_t mat)
{
    char str[RLC_BN_BITS + 2];
    int strlen;

    fprintf(file, "%u %u\n", mat_dim(mat), mat_dim(mat));

    if (!mat_is_empty(mat))
    {
        if (format < 2 || format > 64) format = 16;

        for (uint8_t i = 0; i < mat_dim(mat); i++)
        {
            for (uint8_t j = 0; j < mat_dim(mat); j++)
            {
                strlen = bn_size_str(MAT(mat, i, j), format);
                bn_write_str(str, strlen, MAT(mat, i, j), format);
                fprintf(file, "%s ", str);
            }
            fprintf(file, "\n");
        }
    }

    /* TODO Calculer proprement le nombre des elements ecrits dans le flux file */
    return 2 + mat_dim(mat);
}

void mat_clear(mat_t mat)
{
    if (!mat_is_empty(mat))
    {
        for (int i = 0; i < mat_dim(mat) * mat_dim(mat); i++)
            bn_free(mat->entries[i]);

        free(mat->entries);
        mat->entries = NULL;
    }

void bn_vect_clear(bn_vect_t vect)
{
  if (vect->dim != 0)
  {
    for (uint8_t i = 0; i < vect->dim; i++) bn_free(vect->coord[i]);

    free(vect->coord);
    vect->coord = NULL;
  }
}

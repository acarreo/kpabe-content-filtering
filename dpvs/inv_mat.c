#include "matrix.h"

/*****************************************************************************/
/********************* Inverse of matrices of dim <= 2 ***********************/
static inline bool invert_matrix_1x1(mat_t dest, const mat_t src)
{
    bn_mod_inv(GET(dest, 0, 0), GET(src, 0, 0), Fq);
    return true;
}

bool invert_matrix_2x2(mat_t dest, const mat_t src)
{
    bool is_inversible = false;

    bn_t a, b, det;

    bn_null(a);
    bn_null(b);
    bn_null(det);

    RLC_TRY {
        bn_new(a);
        bn_new(b);
        bn_new(det);

        bn_mod_mul(a, GET(src, 0, 0), GET(src, 1, 1), Fq);
        bn_mod_mul(b, GET(src, 0, 1), GET(src, 1, 0), Fq);
        bn_mod_sub(det, a, b, Fq);
        
        if (!bn_is_zero(det))
        {
            bn_mod_inv(det, det, Fq);

            bn_mod_mul(GET(dest, 0, 0), GET(src, 1, 1), det, Fq);
            bn_mod_mul(GET(dest, 1, 1), GET(src, 0, 0), det, Fq);

            bn_neg(det, det);
            bn_mod(det, det, Fq);

            bn_mod_mul(GET(dest, 0, 1), GET(src, 0, 1), det, Fq);
            bn_mod_mul(GET(dest, 1, 0), GET(src, 1, 0), det, Fq);

            is_inversible = true;
        }
    }
    RLC_CATCH_ANY {
        fprintf(stderr, "Errors during inversion matrix\n");
        is_inversible = false;
    }
    RLC_FINALLY {
        bn_free(det);
        bn_free(a);
        bn_free(b);
    }

    return is_inversible;
}
/*****************************************************************************/

bool LU_decompose(mat_t mat, uint8_t *P)
{
  bool is_decompose = false;

  int i, j, k, imax;
  int dim = mat_dim(mat);

  bn_t max_A, max_tmp, tmp;
  bn_vect_t row_norm;

  imax = 0;

  bn_null(tmp);
  bn_null(max_A);
  bn_null(max_tmp);

  RLC_TRY {
    bn_new(tmp);
    bn_new(max_A);
    bn_new(max_tmp);

    bn_vect_init(row_norm, dim);

    for (i = 0; i < dim; i++)
    {
      bn_zero(max_A);
      for (j = 0; j < dim; j++) {
        if (bn_cmp(GET(mat, i, j), max_A) == RLC_GT) {
          bn_copy(max_A, GET(mat, i, j));
        }
      }

      /* failure if matrix is degenerate */
      if (bn_is_zero(max_A)) {
        fprintf(stderr, "LU_decompose(): row-wise singular matrix!\n");
        RLC_THROW(ERR_NO_VALID);
      }

      bn_mod_inv(row_norm->coord[i], max_A, Fq);
    }

    /* For each of the columns, starting from the left ... */
    for (j = 0; j < dim; j++)
    {
      /* Calculate the Upper part of the matrix:  i < j :   */
      for (i = 0; i < j; i++) {
        for (k = 0; k < i; k++) {
          bn_mod_mul(tmp, GET(mat, i, k), GET(mat, k, j), Fq);
          bn_mod_sub(GET(mat, i, j), GET(mat, i, j), tmp, Fq);
        }
      }

      bn_zero(max_A);

      /* Calculate the Lower part of the matrix:  i <= j :   */
      for (i = j; i < dim; i++)
      {
        for (k = 0; k < j; k++) {
          bn_mod_mul(tmp, GET(mat, i, k), GET(mat, k, j), Fq);
          bn_mod_sub(GET(mat, i, j), GET(mat, i, j), tmp, Fq);
        }

        bn_mod_mul(max_tmp, GET(mat, i, j), row_norm->coord[i], Fq);

        if (bn_cmp(max_tmp, max_A) == RLC_GT || bn_cmp(max_tmp, max_A) == RLC_EQ) {
          bn_copy(max_A, max_tmp);
          imax = i;
        }
      }

      if (imax != j)
      {
        if (j == (dim-2) && bn_is_zero(GET(mat, j, j+1))) imax = j;
        else
        {
          for (k = 0; k < dim; k++) {
            bn_copy(max_tmp, GET(mat, j, k));
            bn_copy(GET(mat, j, k), GET(mat, imax, k));
            bn_copy(GET(mat, imax, k), max_tmp);
          }

          bn_copy(row_norm->coord[imax], row_norm->coord[j]);
        }
      }

      P[j] = imax;

      if (bn_is_zero(GET(mat, j, j))) {
        RLC_THROW(ERR_NO_VALID);
      }

      if (j != dim-1)
      {
        bn_mod_inv(max_tmp, GET(mat, j, j), Fq);
        for (i = j+1; i < dim; i++) {
          bn_mod_mul(GET(mat, i, j), GET(mat, i, j), max_tmp, Fq);
        }
      }
    }

    is_decompose = true;
  }
  RLC_CATCH_ANY {
    fprintf(stderr, "Errors with LU decomposition\n");
  }
  RLC_FINALLY {
    bn_free(tmp);
    bn_free(max_A);
    bn_free(max_tmp);
    bn_vect_clear(row_norm);
  }

  return is_decompose;
}

void LU_substitution(bn_vect_t B, const mat_t mat, const uint8_t *P)
{
  int dim = mat_dim(mat);

  bn_t tmp, tmp2;

  bn_null(tmp);
  bn_null(tmp2);

  RLC_TRY {
    bn_new(tmp);
    bn_new(tmp2);

    for (int i = 0; i < dim; i++)
    {
      bn_copy(tmp, B->coord[P[i]]);
      bn_copy(B->coord[P[i]], B->coord[i]);

      for (int j = i-1; j >= 0; j--) {
        bn_mod_mul(tmp2, GET(mat, i, j), B->coord[j], Fq);
        bn_mod_sub(tmp, tmp, tmp2, Fq);
      }
      bn_copy(B->coord[i], tmp);
    }

    for (int i = dim-1; i >= 0; i--) {
      for (int j = i+1; j < dim; j++) {
        bn_mod_mul(tmp, GET(mat, i, j), B->coord[j], Fq);
        bn_mod_sub(B->coord[i], B->coord[i], tmp, Fq);
      }

      bn_mod_inv(tmp, GET(mat, i, i), Fq);
      bn_mod_mul(B->coord[i], B->coord[i], tmp, Fq);
    }
  }
  RLC_CATCH_ANY {
    fprintf(stderr, "Problem with LU_substitution()\n");
  }
  RLC_FINALLY {
    bn_free(tmp);
    bn_free(tmp2);
  }
}

bool LU_invert_matrix(mat_t inv_A, const mat_t A)
{
  bool is_inversible = false;

  bn_t one;
  mat_t A_tmp;
  int dim = mat_dim(A);
  uint8_t *P = malloc(dim * sizeof(uint8_t));
  bn_vect_t vect_B;

  bn_null(one);
  bn_new(one);
  bn_set_dig(one, 1);

  if (mat_init(A_tmp, dim) && bn_vect_init(vect_B, dim))
  {
    mat_copy(A_tmp, A);

    if (LU_decompose(A_tmp, P))
    {
      for (uint8_t i = 0; i < dim; i++)
      {
        for (uint8_t j = 0; j < dim; j++) bn_zero(vect_B->coord[j]);
        bn_copy(vect_B->coord[i], one);

        LU_substitution(vect_B, A_tmp, P);

        for (uint8_t j = 0; j < dim; j++)
          bn_copy(GET(inv_A, j, i), vect_B->coord[j]);
      }

      is_inversible = true;
    }
  }

  free(P);
  bn_vect_clear(vect_B);
  mat_clear(A_tmp);

  return is_inversible;
}

bool invert_matrix(mat_t dest, const mat_t src)
{
  if (mat_is_empty(src)) {
    fprintf(stderr, "Empty matrix is not invertible\n");
    return false;
  }

  if (mat_dim(src) == 1) {return invert_matrix_1x1(dest, src);}
  if (mat_dim(src) == 2) {return invert_matrix_2x2(dest, src);}

  /***************************************************************************/
  /* If matrix dim is > 2, we use LU decomposition to compute inverse matrix */
  return LU_invert_matrix(dest, src);
}

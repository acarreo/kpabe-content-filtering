#include "matrix.h"

/************************ STATIC FUNCTION PROTOTYPES ************************/
static inline bool invert_matrix_1x1(mat_t dest, const mat_t src);
static bool invert_matrix_2x2(mat_t dest, const mat_t src);
static bool LU_invert_matrix(mat_t inv_A, const mat_t A);
/****************************************************************************/

bool mat_init(mat_t mat, uint8_t dim)
{
  bool ret = false;
  mat->dim = dim;

  int mat_size = dim * dim;
  if ((ret = (dim != 0)))
  {
    if ((mat->entries = malloc(mat_size * sizeof(bn_t))) == NULL)
      return _error_alloc_fail_();

    int i = 0;
    RLC_TRY {
      for (; i < mat_size; i++)
      {
        bn_null(mat->entries[i]);
        bn_new(mat->entries[i]);
      }
    } RLC_CATCH_ANY {
      mat->dim = i;   /* clear the first i elements of mat properly */
      mat_clear(mat);
      return _error_alloc_fail_();
    }
  }

  return ret;
}

bool bn_vect_init(bn_vect_t vect, uint8_t dim)
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
    for (int i = 0; i < mat_dim(A) * mat_dim(A); i++)
      test_cmp &= (bn_cmp(A->entries[i], B->entries[i]) == RLC_EQ);
  }

  return test_cmp;
}

bool mat_is_zero(const mat_t mat)
{
  bool is_zero = true;

  for (int i = 0; i < mat_dim(mat) * mat_dim(mat); i++)
    is_zero &= bn_is_zero(mat->entries[i]);

  return is_zero;
}

void mat_eye(mat_t mat, uint8_t dim)
{
  if (mat_init(mat, dim))
  {
    bn_t one;

    bn_null(one);
    bn_new(one);
    bn_set_dig(one, 1);

    for (uint8_t i = 0; i < mat_dim(mat); i++)
      bn_copy(GET(mat, i, i), one);

    bn_free(one);
  }
}

void mat_transpose(mat_t dest, const mat_t src)
{
  if (mat_dim(dest) == mat_dim(src))
    for (uint8_t i = 0; i < mat_dim(src); i++)
      for (uint8_t j = 0; j < mat_dim(src); j++)
        bn_copy(GET(dest, i, j), GET(src, j, i));
}

void mat_copy(mat_t dest, const mat_t src)
{
  if (mat_dim(dest) == mat_dim(src))
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
            bn_mod_mul(tmp, GET(A, i, k), GET(B, k, j), Fq);
            bn_mod_add(sum, sum, tmp, Fq);
          }
          bn_copy(GET(dest, i, j), sum);
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
  if (!mat_is_empty(A) && mat_dim(A) == mat_dim(inv_A))
    do { mat_rand(A); } while (!mat_invert_matrix(inv_A, A));
}

bool mat_get_row(bn_vect_t row, const mat_t mat, uint8_t index)
{
  if (index >= mat_dim(mat) || mat_dim(mat) != row->dim) return false;

  for (uint8_t j = 0; j < mat_dim(mat); j++)
    bn_copy(row->coord[j], GET(mat, index, j));

  return true;
}

void bn_inner_product(bn_t ip, const bn_vect_t vect1, const bn_vect_t vect2)
{
  uint8_t dim = vect1->dim;

  bn_t tmp;

  bn_null(tmp);

  if (dim == vect2->dim)
  {
    bn_new(tmp);
    bn_zero(ip);
    for (uint8_t i = 0; i < dim; i++)
    {
      bn_mod_mul(tmp, vect1->coord[i], vect2->coord[i], Fq);
      bn_mod_add(ip, ip, tmp, Fq);
    }
  }

  bn_free(tmp);
}

bool mat_is_dual_pair(const mat_t mat, const mat_t dual_mat)
{
  if (mat->dim != dual_mat->dim) return false;

  bool is_dual_pair = true;
  const uint8_t dim = mat->dim;

  bn_vect_t vect[dim];
  bn_vect_t dual_vect[dim];

  bn_t ip, one;

  bn_null(ip);
  bn_null(one);

  bn_new(ip);
  bn_new(one);
  bn_set_dig(one, 1);

  for (uint8_t i = 0; i < dim; i++)
  {
    bn_vect_init(vect[i], dim);
    bn_vect_init(dual_vect[i], dim);

    mat_get_row(vect[i], mat, i);
    mat_get_row(dual_vect[i], dual_mat, i);
  }

  for (uint8_t i = 0; i < dim; i++)
  {
    for (uint8_t j = 0; j < dim; j++) {
      bn_inner_product(ip, vect[i], dual_vect[j]);

      if (i == j)
        is_dual_pair &= (bn_cmp(ip, one) == RLC_EQ);
      else
        is_dual_pair &= bn_is_zero(ip);
    }
  }

  for (uint8_t i = 0; i < dim; i++) {
    bn_vect_clear(vect[i]);
    bn_vect_clear(dual_vect[i]);
  }
  bn_free(one);

  return is_dual_pair;
}

bool mat_rand_dual_mat(mat_t mat, mat_t dual_mat, uint8_t dim)
{
  bool ret = false;
  mat_t tmp;

  if (mat_init(mat, dim) && mat_init(dual_mat, dim) && mat_init(tmp, dim))
  {
    do {
      mat_rand_inv(mat, tmp);
      mat_transpose(dual_mat, tmp);
      mat_clear(tmp);
    } while (!(ret = mat_is_dual_pair(mat, dual_mat)));
  }

  return ret;
}

bool mat_invert_matrix(mat_t dest, const mat_t src)
{
  if (mat_is_empty(src)) {
    fprintf(stderr, "Empty matrix is not invertible\n");
    return false;
  }

  /* If matrix dim is <= 2 */
  if (mat_dim(src) == 1) {return invert_matrix_1x1(dest, src);}
  if (mat_dim(src) == 2) {return invert_matrix_2x2(dest, src);}

  /* If matrix dim is > 2, we use LU decomposition to compute inverse matrix */
  return LU_invert_matrix(dest, src);
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
        strlen = bn_size_str(GET(mat, i, j), format);
        bn_write_str(str, strlen, GET(mat, i, j), format);
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

/*****************************************************************************
****************************** STATIC FUNCTIONS ******************************
******************************************************************************/

/********************* Inverse of matrices of dim <= 2 ***********************/
static inline bool invert_matrix_1x1(mat_t dest, const mat_t src)
{
  bn_mod_inv(GET(dest, 0, 0), GET(src, 0, 0), Fq);
  return true;
}

static bool invert_matrix_2x2(mat_t dest, const mat_t src)
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

/********** Inverse of matrices of dim > 2 using LU decomposition **********/
static bool LU_decompose(mat_t mat, uint8_t *P)
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

static void LU_substitution(bn_vect_t B, const mat_t mat, const uint8_t *P)
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

static bool LU_invert_matrix(mat_t inv_A, const mat_t A)
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

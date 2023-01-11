#include "dpvs.h"

bool dpvs_init_base_vect(g1_vect_t bvect, uint8_t dim)
{
  bool ret;

  bvect->dim = dim;

  if ((ret = (dim != 0)))
  {
    if ((bvect->coord = malloc(dim * sizeof(g1_t))) == NULL)
      return _error_alloc_fail_();

    int i = 0;
    RLC_TRY {
      for (; i < dim; i++) {
        g1_null(bvect->coord[i]);
        g1_new(bvect->coord[i]);
      }
    }
    RLC_CATCH_ANY {
      bvect->dim = i;
      dpvs_clear_base_vect(bvect);
      ret = _error_alloc_fail_();
    }
  }

  return ret;
}

bool dpvs_init_dual_base_vect(g2_vect_t db_vect, uint8_t dim)
{
  bool ret;

  db_vect->dim = dim;

  if ((ret = (dim != 0)))
  {
    if ((db_vect->coord = malloc(dim * sizeof(g2_t))) == NULL)
      return _error_alloc_fail_();

    int i = 0;
    RLC_TRY {
      for (; i < dim; i++) {
        g2_null(db_vect->coord[i]);
        g2_new(db_vect->coord[i]);
      }
    }
    RLC_CATCH_ANY {
      db_vect->dim = i;
      dpvs_clear_dual_base_vect(db_vect);
      ret = _error_alloc_fail_();
    }
  }

  return ret;
}

bool dpvs_init(dpvs_t dpvs, uint8_t dim)
{
  bool ret = false;

  dpvs->dim = dim;

  if ((ret = (dim != 0)))
  {
    if ((dpvs->base = malloc(dim * sizeof(g1_vect_st*))) == NULL ||
        (dpvs->dual_base = malloc(dim * sizeof(g2_vect_st*))) == NULL)
    {
      return _error_alloc_fail_();
    }

    int i = 0;
    RLC_TRY {
      for (; i < dim; i++)
      {
        if ((dpvs->base[i] = malloc(sizeof(g1_vect_st))) == NULL ||
            !dpvs_init_base_vect(dpvs->base[i], dim) ||
            (dpvs->dual_base[i] = malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(dpvs->dual_base[i], dim))
        {
          RLC_THROW(ERR_MAX);
        }
      }
    }
    RLC_CATCH_ANY {
      dpvs->dim = i;
      dpvs_clear(dpvs);
      ret = _error_alloc_fail_();
    }
  }

  return ret;
}

bool dpvs_gen(dpvs_t dpvs, uint8_t dim)
{
  bool ret = false;
  mat_t mat, dual_mat;
  bn_vect_t row, drow;

  if (dpvs_init(dpvs, dim) && dpvs_gen_matrices(mat, dual_mat, dim))
  {
    if (bn_vect_init(row, dim) && bn_vect_init(drow, dim))
    {
      for (uint8_t i = 0; i < dim; i++)
      {
        dpvs_get_mat_row(row, mat, i);
        dpvs_get_mat_row(drow, dual_mat, i);
        for (uint8_t j = 0; j < dim; j++)
        {
          g1_mul_gen(dpvs->base[i]->coord[j], row->coord[j]);
          g2_mul_gen(dpvs->dual_base[i]->coord[j], drow->coord[j]);
        }
      }
      ret = true;
    }
    else {
      fprintf(stderr, "[Errors] dpvs_gen: vectors initialization failled\n");
    }
  }
  else {
    fprintf(stderr, "[Errors] dpvs_gen: dpvs entries, initialization failled\n");
  }

  bn_vect_clear(row);
  bn_vect_clear(drow);
  mat_clear(mat);
  mat_clear(dual_mat);

  return ret;
}

void dpvs_k_mul_vect(g1_vect_t dest, const g1_vect_t src, bn_t k)
{
  if (dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g1_mul(dest->coord[i], src->coord[i], k);
  }
}

void dpvs_k_mul_dual_vect(g2_vect_t dest, const g2_vect_t src, bn_t k)
{
  if (dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g2_mul(dest->coord[i], src->coord[i], k);
  }
}

void dpvs_add_vect(g1_vect_t dest, const g1_vect_t src1, const g1_vect_t src2)
{
  if (src1->dim == src2->dim && dest->dim == src2->dim) {
    for (uint8_t i = 0; i < dest->dim; i++) {
      g1_add(dest->coord[i], src1->coord[i], src2->coord[i]);
    }
  }
}

void dpvs_add_dual_vect(g2_vect_t dest, const g2_vect_t src1, const g2_vect_t src2)
{
  if (src1->dim == src2->dim && dest->dim == src2->dim) {
    for (uint8_t i = 0; i < dest->dim; i++) {
      g2_add(dest->coord[i], src1->coord[i], src2->coord[i]);
    }
  }
}

void dpvs_g1_vect_copy (g1_vect_t dest, g1_vect_t src) {
  if (dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g1_copy(dest->coord[i], src->coord[i]);
}

void dpvs_g2_vect_copy (g2_vect_t dest, g2_vect_t src) {
  if (dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g2_copy(dest->coord[i], src->coord[i]);
}

/*****************************************************************************/
void dpvs_clear_base_vect(g1_vect_t bvect)
{
  if (bvect->dim != 0)
  {
    for (uint8_t i = 0; i < bvect->dim; i++) g1_free(bvect->coord[i-1]);
    free(bvect->coord);
    bvect->coord = NULL;
  }
}

void dpvs_clear_dual_base_vect(g2_vect_t db_vect)
{
  if (db_vect->dim != 0)
  {
    for (uint8_t i = 0; i < db_vect->dim; i++) g2_free(db_vect->coord[i]);
    free(db_vect->coord);
    db_vect->coord = NULL;
  }
}

void dpvs_clear(dpvs_t dpvs)
{
  if (dpvs->dim != 0 && dpvs->base && dpvs->dual_base)
  {
    for (uint8_t i = 0; i < dpvs->dim; i++)
    {
      dpvs_clear_base_vect(dpvs->base[i]);
      dpvs_clear_dual_base_vect(dpvs->dual_base[i]);
      free(dpvs->base[i]);
      free(dpvs->dual_base[i]);
    }
    free(dpvs->base);
    free(dpvs->dual_base);
  }
}

/*****************************************************************************
 * the bilinear group setting is (G1, G2, Gt, g1, g2, gt, e, q) where
 * G1, G2, Gt are groups of prime order q, g1 and g2 element of G1, G2 resp.
 * e : G1 x G2 --> Gt a pairing and gt = e(g1, g2).
 * We choose g1 (resp. g2) a generator of G1 (resp. G2).
 ****************************************************************************/
void set_params(void)
{
  g1_t g1;
  g2_t g2;

  g1_null(g1);
  g2_null(g2);
  gt_null(params->gt);
  bn_null(params->q);

  RLC_TRY {
    g1_new(g1);
    g2_new(g2);
    gt_new(params->gt);
    bn_new(params->q);

    g1_get_gen(g1);
    g2_get_gen(g2);
    pc_map(params->gt, g1, g2);
    pc_get_ord(params->q);

    bn_copy(Fq, params->q);
  }
  RLC_CATCH_ANY {
    fprintf(stderr, "Problem with initialization of bilinear group params\n");
    clear_params();
  }
  RLC_FINALLY {
    g1_free(g1);
    g2_free(g2);
  }
}

void clear_params(void)
{
  gt_free(params->gt);
  bn_free(params->q);
  bn_free(Fq);
}

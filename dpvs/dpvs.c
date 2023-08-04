#include "dpvs.h"

G1_VS_VECT dpvs_create_g1_vect(uint8_t dim) {
  G1_VS_VECT vect = NULL;
  
  if (dim != 0 && (vect = (G1_VS_VECT) malloc(sizeof(g1_vect_t)))) {
    vect->dim = dim;
    if ((vect->coord = (g1_t *) malloc(dim * sizeof(g1_t))) == NULL) {
      free(vect);
      _error_alloc_fail_();
    }
    else {
      int i = 0;
      RLC_TRY {
        for (; i < dim; i++) {
          g1_null(vect->coord[i]);
          g1_new(vect->coord[i]);
        }
      }
      RLC_CATCH_ANY {
        vect->dim = i;
        dpvs_clear_g1_vect(vect);
        _error_alloc_fail_();
      }
    }
  }

  return vect;
}

G2_VS_VECT dpvs_create_g2_vect(uint8_t dim) {
  G2_VS_VECT vect = NULL;
  
  if (dim != 0 && (vect = (G2_VS_VECT) malloc(sizeof(g2_vect_t)))) {
    vect->dim = dim;
    if ((vect->coord = (g2_t *) malloc(dim * sizeof(g2_t))) == NULL) {
      free(vect);
      _error_alloc_fail_();
    }
    else {
      int i = 0;
      RLC_TRY {
        for (; i < dim; i++) {
          g2_null(vect->coord[i]);
          g2_new(vect->coord[i]);
        }
      }
      RLC_CATCH_ANY {
        vect->dim = i;
        dpvs_clear_g2_vect(vect);
        _error_alloc_fail_();
      }
    }
  }

  return vect;
}

G1_VS_BASE dpvs_create_g1_base(uint8_t dim)
{
  G1_VS_BASE vect = NULL;

  if ((vect = (G1_VS_BASE) malloc(dim * sizeof(G1_VS_VECT)))) {
    for (uint8_t i = 0; i < dim; i++) {
      if ((vect[i] = dpvs_create_g1_vect(dim)) == NULL) {
        for (; i > 0; i--) free(vect[i-1]);
        free(vect);
        _error_alloc_fail_();
        break;
      }
    }
  }

  return vect;
}

G2_VS_BASE dpvs_create_g2_base(uint8_t dim)
{
  G2_VS_BASE vect = NULL;

  if ((vect = (G2_VS_BASE) malloc(dim * sizeof(G2_VS_VECT)))) {
    for (uint8_t i = 0; i < dim; i++) {
      if ((vect[i] = dpvs_create_g2_vect(dim)) == NULL) {
        for (; i > 0; i--) free(vect[i-1]);
        free(vect);
        _error_alloc_fail_();
        break;
      }
    }
  }

  return vect;
}

dpvs_t* dpvs_create_bases(uint8_t dim)
{
  dpvs_t* dpvs = NULL;

  if ((dpvs = (dpvs_t*) malloc(sizeof(dpvs_t)))) {
    if ((dpvs->base = dpvs_create_g1_base(dim)) == NULL) {
      free(dpvs);
      _error_alloc_fail_();
    }
    else if ((dpvs->dual_base = dpvs_create_g2_base(dim)) == NULL) {
      free(dpvs->base);
      free(dpvs);
      _error_alloc_fail_();
    }
    else {
      dpvs->dim = dim;
    }
  }

  return dpvs;
}

dpvs_t* dpvs_generate_bases(uint8_t dim)
{
  dpvs_t* dpvs = NULL;
  mat_t mat, dual_mat;
  bn_vect_t row, drow;

  if ((dpvs = dpvs_create_bases(dim)) == NULL) {
    fprintf(stderr, "[Errors] generate_dpvs_bases: dpvs initialization failled\n");
    return NULL;
  }

  if (dpvs_gen_matrices(mat, dual_mat, dim)) {
    if (bn_vect_init(row, dim) && bn_vect_init(drow, dim)) {
      for (uint8_t i = 0; i < dim; i++) {
        dpvs_get_mat_row(row, mat, i);
        dpvs_get_mat_row(drow, dual_mat, i);
        for (uint8_t j = 0; j < dim; j++) {
          g1_mul_gen(dpvs->base[i]->coord[j], row->coord[j]);
          g2_mul_gen(dpvs->dual_base[i]->coord[j], drow->coord[j]);
        }
      }
    }
    else {
      fprintf(stderr, "[Errors] generate_dpvs_bases: vectors initialization failled\n");
      dpvs_clear(dpvs);
    }
  }
  else {
    fprintf(stderr, "[Errors] generate_dpvs_bases: dpvs entries, initialization failled\n");
    dpvs_clear(dpvs);
  }

  bn_vect_clear(row);
  bn_vect_clear(drow);
  mat_clear(mat);
  mat_clear(dual_mat);

  return dpvs;
}

void dpvs_k_mul_g1_vect(G1_VS_VECT dest, const G1_VS_VECT src, bn_t k)
{
  if (dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g1_mul(dest->coord[i], src->coord[i], k);
  }
}

void dpvs_k_mul_g2_vect(G2_VS_VECT dest, const G2_VS_VECT src, bn_t k)
{
  if (dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g2_mul(dest->coord[i], src->coord[i], k);
  }
}

void dpvs_add_g1_vect(G1_VS_VECT dest, const G1_VS_VECT src1, const G1_VS_VECT src2)
{
  if (src1->dim == src2->dim && dest->dim == src2->dim) {
    for (uint8_t i = 0; i < dest->dim; i++) {
      g1_add(dest->coord[i], src1->coord[i], src2->coord[i]);
    }
  }
}

void dpvs_add_g2_vect(G2_VS_VECT dest, const G2_VS_VECT src1, const G2_VS_VECT src2)
{
  if (src1->dim == src2->dim && dest->dim == src2->dim) {
    for (uint8_t i = 0; i < dest->dim; i++) {
      g2_add(dest->coord[i], src1->coord[i], src2->coord[i]);
    }
  }
}

void dpvs_copy_g1_vect (G1_VS_VECT dest, G1_VS_VECT src) {
  if (dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g1_copy(dest->coord[i], src->coord[i]);
}

void dpvs_copy_g2_vect (G2_VS_VECT dest, G2_VS_VECT src) {
  if (dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g2_copy(dest->coord[i], src->coord[i]);
}

void dpvs_clear_g1_vect(G1_VS_VECT vect)
{
  if (vect->dim != 0) {
    for (uint8_t i = 0; i < vect->dim; i++) g1_free(vect->coord[i]);

    free(vect->coord);
    free(vect);
  }
}

void dpvs_clear_g2_vect(G2_VS_VECT vect)
{
  if (vect->dim != 0) {
    for (uint8_t i = 0; i < vect->dim; i++) g2_free(vect->coord[i]);

    free(vect->coord);
    free(vect);
  }
}

void dpvs_clear(dpvs_t* dpvs)
{
  if (dpvs->dim != 0 && dpvs->base && dpvs->dual_base)
  {
    for (uint8_t i = 0; i < dpvs->dim; i++)
    {
      dpvs_clear_g1_vect(dpvs->base[i]);
      dpvs_clear_g2_vect(dpvs->dual_base[i]);
    }
    free(dpvs->base);
    free(dpvs->dual_base);
    free(dpvs);
  }
}

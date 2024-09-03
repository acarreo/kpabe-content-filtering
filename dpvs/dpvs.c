#include "dpvs.h"

g1_vector_ptr dpvs_create_g1_vect(uint8_t dim) {
  g1_vector_ptr vect = NULL;
  
  if (dim != 0 && (vect = (g1_vector_ptr) malloc(sizeof(g1_vector_t)))) {
    vect->dim = dim;
    if ((vect->elements = (g1_t *) malloc(dim * sizeof(g1_t))) == NULL) {
      free(vect);
      _error_alloc_fail_();
    }
    else {
      int i = 0;
      RLC_TRY {
        for (; i < dim; i++) {
          g1_null(vect->elements[i]);
          g1_new(vect->elements[i]);
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

g2_vector_ptr dpvs_create_g2_vect(uint8_t dim) {
  g2_vector_ptr vect = NULL;
  
  if (dim != 0 && (vect = (g2_vector_ptr) malloc(sizeof(g2_vector_t)))) {
    vect->dim = dim;
    if ((vect->elements = (g2_t *) malloc(dim * sizeof(g2_t))) == NULL) {
      free(vect);
      _error_alloc_fail_();
    }
    else {
      int i = 0;
      RLC_TRY {
        for (; i < dim; i++) {
          g2_null(vect->elements[i]);
          g2_new(vect->elements[i]);
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
  if (dim == 0) return NULL;

  G1_VS_BASE vect = (G1_VS_BASE) malloc(dim * sizeof(g1_vector_ptr));
  if (vect != NULL) {
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
  if (dim == 0) return NULL;

  G2_VS_BASE vect = (G2_VS_BASE) malloc(dim * sizeof(g2_vector_ptr));
  if (vect != NULL) {
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
  if (dim == 0) return NULL;

  dpvs_t* dpvs = (dpvs_t*) malloc(sizeof(dpvs_t));
  if (dpvs == NULL) return NULL;
  dpvs->dim = dim;

  dpvs->base = dpvs_create_g1_base(dim);
  if (dpvs->base == NULL) {
    free(dpvs);
    return NULL;
  }

  dpvs->dual_base = dpvs_create_g2_base(dim);
  if (dpvs->dual_base == NULL) {
    free(dpvs->base); free(dpvs);
    return NULL;
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
          g1_mul_gen(dpvs->base[i]->elements[j], row->elements[j]);
          g2_mul_gen(dpvs->dual_base[i]->elements[j], drow->elements[j]);
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

void dpvs_k_mul_g1_vect(g1_vector_ptr dest, const g1_vector_ptr src, const bn_t k)
{
  if (dest && src && dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g1_mul(dest->elements[i], src->elements[i], k);
  }
}

void dpvs_k_mul_g2_vect(g2_vector_ptr dest, const g2_vector_ptr src, const bn_t k)
{
  if (dest && src && dest->dim == src->dim) {
    for (uint8_t i = 0; i < dest->dim; i++)
      g2_mul(dest->elements[i], src->elements[i], k);
  }
}

void dpvs_add_g1_vect(g1_vector_ptr dest, const g1_vector_ptr src1, const g1_vector_ptr src2)
{
  if (dest && src1 && src2) {
    if (src1->dim == src2->dim && dest->dim == src2->dim) {
      for (uint8_t i = 0; i < dest->dim; i++) {
        g1_add(dest->elements[i], src1->elements[i], src2->elements[i]);
      }
    }
  }
}

void dpvs_add_g2_vect(g2_vector_ptr dest, const g2_vector_ptr src1, const g2_vector_ptr src2)
{
  if (dest && src1 && src2) {
    if (src1->dim == src2->dim && dest->dim == src2->dim) {
      for (uint8_t i = 0; i < dest->dim; i++) {
        g2_add(dest->elements[i], src1->elements[i], src2->elements[i]);
      }
    }
  }
}

void dpvs_copy_g1_vect (g1_vector_ptr dest, g1_vector_ptr src) {
  if (dest && src && dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g1_copy(dest->elements[i], src->elements[i]);
}

void dpvs_copy_g2_vect (g2_vector_ptr dest, g2_vector_ptr src) {
  if (dest && src && dest->dim == src->dim)
    for (uint8_t i = 0; i < src->dim; i++)
      g2_copy(dest->elements[i], src->elements[i]);
}

bool dpvs_compare_g1_vect(const g1_vector_ptr vect1, const g1_vector_ptr vect2) {
  if (!vect1 || !vect2)
    return (vect1 == vect2);

  if (vect1->dim != vect2->dim)
    return false;

  for (uint8_t i = 0; i < vect1->dim; i++)
    if (g1_cmp(vect1->elements[i], vect2->elements[i]) != RLC_EQ)
      return false;

  return true;
}

bool dpvs_compare_g2_vect(const g2_vector_ptr vect1, const g2_vector_ptr vect2) {
  if (!vect1 || !vect2)
    return (vect1 == vect2);

  if (vect1->dim != vect2->dim)
    return false;

  for (uint8_t i = 0; i < vect1->dim; i++)
    if (g2_cmp(vect1->elements[i], vect2->elements[i]) != RLC_EQ)
      return false;

  return true;
}

void dpvs_clear_g1_vect(g1_vector_ptr vect)
{
  if (vect) {
    if (vect->elements) {
      for (uint8_t i = 0; i < vect->dim; i++) g1_free(vect->elements[i]);

      free(vect->elements);
    }
    free(vect);
  }
}

void dpvs_clear_g2_vect(g2_vector_ptr vect)
{
  if (vect) {
    if (vect->elements) {
      for (uint8_t i = 0; i < vect->dim; i++) g2_free(vect->elements[i]);

      free(vect->elements);
    }
    free(vect);
  }
}

void dpvs_clear(dpvs_t* dpvs)
{
  if (dpvs) {
    if (dpvs->base && dpvs->dual_base) {
      for (uint8_t i = 0; i < dpvs->dim; i++)
      {
        dpvs_clear_g1_vect(dpvs->base[i]);
        dpvs_clear_g2_vect(dpvs->dual_base[i]);
      }
      free(dpvs->base);
      free(dpvs->dual_base);
    }
    free(dpvs);
  }
}

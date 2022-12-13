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

      }
    } RLC_CATCH_ANY {
      for (; i > 0; i--) bn_free(vect->coord[i-1]);
      free(vect->coord);
      return _error_mat_alloc_();
    }
  }

  return true;
}

void get_vect_from_base(vect_t vect, const base_t base, uint8_t index)
{
  if (index < base->dim)
    for (uint8_t j = 0; j < base->dim; j++)
      bn_copy(vect->coord[j], GET(base, index, j));
}

void vect_clear(vect_t vect)
{
  if (vect->dim != 0)
  {
    for (uint8_t i = 0; i < vect->dim; i++) bn_free(vect->coord[i]);

    free(vect->coord);
    vect->coord = NULL;
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

}

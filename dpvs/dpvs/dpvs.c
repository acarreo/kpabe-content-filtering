
#include "base.h"

bool vect_init(vect_t vect, uint8_t dim)
{
  vect->dim = dim;

  if (dim != 0)
  {
    if ((vect->coord = malloc(dim * sizeof(bn_t))) == NULL)
      return _error_mat_alloc_();

    int i = 0;
    RLC_TRY {
      for (; i < dim; i++)
      {
        bn_null(vect->coord[i]);
        bn_new(vect->coord[i]);
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
}

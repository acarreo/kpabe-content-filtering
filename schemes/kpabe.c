#include "kpabe.h"

bool ABE_pub_key_new(ABE_pub_key_t pk)
{
  if (dpvs_init_base_vect(pk->d1, ND) && dpvs_init_base_vect(pk->d3, ND) &&
      dpvs_init_base_vect(pk->g1, NG) && dpvs_init_base_vect(pk->g2, NG) &&
      dpvs_init_base_vect(pk->f1, NF) && dpvs_init_base_vect(pk->f2, NF) &&
      dpvs_init_base_vect(pk->f3, NF) && dpvs_init_base_vect(pk->h1, NH) &&
      dpvs_init_base_vect(pk->h2, NH) && dpvs_init_base_vect(pk->h3, NH))
  {
    return true;
  }

  return false;
}

bool ABE_ms_key_new(ABE_ms_key_t msk)
{
  if (dpvs_init_dual_base_vect(msk->d1, ND) && dpvs_init_dual_base_vect(msk->d3, ND) &&
      dpvs_init_dual_base_vect(msk->g1, NG) && dpvs_init_dual_base_vect(msk->g2, NG) &&
      dpvs_init_dual_base_vect(msk->f1, NF) && dpvs_init_dual_base_vect(msk->f2, NF) &&
      dpvs_init_dual_base_vect(msk->f3, NF) && dpvs_init_dual_base_vect(msk->h1, NH) &&
      dpvs_init_dual_base_vect(msk->h2, NH) && dpvs_init_dual_base_vect(msk->h3, NH))
  {
    return true;
  }

  return false;
}

bool ABE_setup(ABE_pub_key_t pk, ABE_ms_key_t msk)
{
  bool ret = true;
  dpvs_t D, G, F, H;

  if (ABE_pub_key_new(pk) && ABE_ms_key_new(msk)) {
    if (dpvs_gen(D, ND) && dpvs_gen(G, NG) && dpvs_gen(F, NF) && dpvs_gen(H, NH))
    {
      /* Public key */
      dpvs_g1_vect_copy(pk->d1, D->base[0]);
      dpvs_g1_vect_copy(pk->d3, D->base[2]);

      dpvs_g1_vect_copy(pk->g1, G->base[0]);
      dpvs_g1_vect_copy(pk->g2, G->base[1]);

      dpvs_g1_vect_copy(pk->f1, F->base[0]);
      dpvs_g1_vect_copy(pk->f2, F->base[1]);
      dpvs_g1_vect_copy(pk->f3, F->base[2]);

      dpvs_g1_vect_copy(pk->h1, H->base[0]);
      dpvs_g1_vect_copy(pk->h2, H->base[1]);
      dpvs_g1_vect_copy(pk->h3, H->base[2]);

      /* Master secret key */
      dpvs_g2_vect_copy(msk->d1, D->dual_base[0]);
      dpvs_g2_vect_copy(msk->d3, D->dual_base[2]);

      dpvs_g2_vect_copy(msk->g1, G->dual_base[0]);
      dpvs_g2_vect_copy(msk->g2, G->dual_base[1]);

      dpvs_g2_vect_copy(msk->f1, F->dual_base[0]);
      dpvs_g2_vect_copy(msk->f2, F->dual_base[1]);
      dpvs_g2_vect_copy(msk->f3, F->dual_base[2]);

      dpvs_g2_vect_copy(msk->h1, H->dual_base[0]);
      dpvs_g2_vect_copy(msk->h2, H->dual_base[1]);
      dpvs_g2_vect_copy(msk->h3, H->dual_base[2]);
    }
    else {
      ret = _error_alloc_fail_();
    }
  }
  else {
    ret = _error_alloc_fail_();
  }

  dpvs_clear(D);
  dpvs_clear(G);
  dpvs_clear(F);
  dpvs_clear(H);

  return ret;
}

void ABE_free_pub_key(ABE_pub_key_t pk)
{
  dpvs_clear_base_vect(pk->d1); dpvs_clear_base_vect(pk->d3);
  dpvs_clear_base_vect(pk->g1); dpvs_clear_base_vect(pk->g2);
  dpvs_clear_base_vect(pk->f1); dpvs_clear_base_vect(pk->f2);
  dpvs_clear_base_vect(pk->f3); dpvs_clear_base_vect(pk->h1);
  dpvs_clear_base_vect(pk->h2); dpvs_clear_base_vect(pk->h3);
}

void ABE_free_ms_key(ABE_ms_key_t msk)
{
  dpvs_clear_dual_base_vect(msk->d1); dpvs_clear_dual_base_vect(msk->d3);
  dpvs_clear_dual_base_vect(msk->g1); dpvs_clear_dual_base_vect(msk->g2);
  dpvs_clear_dual_base_vect(msk->f1); dpvs_clear_dual_base_vect(msk->f2);
  dpvs_clear_dual_base_vect(msk->f3); dpvs_clear_dual_base_vect(msk->h1);
  dpvs_clear_dual_base_vect(msk->h2); dpvs_clear_dual_base_vect(msk->h3);
}

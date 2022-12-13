#include "kp-abe.h"

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

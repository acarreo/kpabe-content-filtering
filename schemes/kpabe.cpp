#include <algorithm>
#include "kpabe.h"

bool ABE_pub_key_init(ABE_pub_key_t pk)
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

bool ABE_ms_key_init(ABE_ms_key_t msk)
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

bool ABE_secret_key_init(ABE_secret_key_t sk, uint size_wl, uint size_bl, uint size_att)
{
  bool ret = false;
  uint i = 0, j = 0, k = 0;

  sk->size_wl = size_wl;
  sk->size_bl = size_bl;
  sk->size_att = size_att;

  if (dpvs_init_dual_base_vect(sk->key_root, ND))
  {
    /* Initialize K_WL in the dual base F* */
    if ((sk->keys_wl = (g2_vect_st**)malloc(size_wl * sizeof(g2_vect_st*))) != NULL) {
      for (; i < size_wl; i++) {
        if ((sk->keys_wl[i] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(sk->keys_wl[i], NF)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }

    /* Initialize K_BL in the dual base G* */
    if ((sk->keys_bl = (g2_vect_st**)malloc(size_bl * sizeof(g2_vect_st*))) != NULL) {
      for (; j < size_bl; j++) {
        if ((sk->keys_bl[j] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(sk->keys_bl[j], NG)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }

    /* Initialize K_att for j in list_att(policy) in the dual base H* */
    if ((sk->keys_att = (g2_vect_st**)malloc(size_att * sizeof(g2_vect_st*))) != NULL) {
      for (; k < size_att; k++) {
        if ((sk->keys_att[k] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(sk->keys_att[k], NH)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }
  }

  ret = (i == size_wl) && (j == size_bl) && (k == size_att);

  return ret;
}

bool ABE_ciphertext_init(ABE_cipher_t cipher, uint size_att)
{
  bool ret = false;
  uint i = 0;

  cipher->size_att = size_att;

  if ((cipher->ctx_att = (g1_vect_st**)malloc(size_att * sizeof(g1_vect_st*))) != NULL) {
    for (; i < size_att; i++) {
      if ((cipher->ctx_att[i] = (g1_vect_st*)malloc(sizeof(g1_vect_st))) == NULL ||
          !dpvs_init_base_vect(cipher->ctx_att[i], NH)) {
            ret = _error_alloc_fail_();
            break;
      }
    }

    if (!dpvs_init_base_vect(cipher->ctx_root, ND) ||
        !dpvs_init_base_vect(cipher->ctx_wl, NF) ||
        !dpvs_init_base_vect(cipher->ctx_bl, NG)) {
          ret = _error_alloc_fail_();
    }
  }

  ret = (i == size_att);

  return ret;
}

bool ABE_gen_params(ABE_pub_key_t pk, ABE_ms_key_t msk)
{
  bool ret = true;
  dpvs_t D, G, F, H;

  if (ABE_pub_key_init(pk) && ABE_ms_key_init(msk)) {
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

void ABE_secret_key_free(ABE_secret_key_t sk)
{
  for (uint i = 0; i < sk->size_wl; i++) {
    dpvs_clear_dual_base_vect(sk->keys_wl[i]);
    free(sk->keys_wl[i]);
  }

  for (uint j = 0; j < sk->size_bl; j++) {
    dpvs_clear_dual_base_vect(sk->keys_bl[j]);
    free(sk->keys_bl[j]);
  }

  for (uint k = 0; k < sk->size_att; k++) {
    dpvs_clear_dual_base_vect(sk->keys_att[k]);
    free(sk->keys_att[k]);
  }

  free(sk->keys_wl);
  free(sk->keys_bl);
  free(sk->keys_att);
  dpvs_clear_dual_base_vect(sk->key_root);
}

void ABE_ciphertext_free(ABE_cipher_t cipher)
{
  for (uint i = 0; i < cipher->size_att; i++) {
    dpvs_clear_base_vect(cipher->ctx_att[i]);
    free(cipher->ctx_att[i]);
  }

  free(cipher->ctx_att);
  dpvs_clear_base_vect(cipher->ctx_root);
  dpvs_clear_base_vect(cipher->ctx_wl);
  dpvs_clear_base_vect(cipher->ctx_bl);

}


bool checkSatisfyPolicy(std::string& policy_str, std::string& attributes,
                        WhiteList_t wl, BlackList_t bl, std::string& url)
{
  std::pair<bool,int> result;
  if (std::find(wl.begin(), wl.end(), url) != wl.end()) {
    result.first = true;
  }
  else if (std::find(bl.begin(), bl.end(), url) == bl.end()) {
    try {
      std::unique_ptr<OpenABEPolicy> policy = createPolicyTree(policy_str);
      std::unique_ptr<OpenABEAttributeList> attrList = createAttributeList(attributes);
      if (policy != nullptr && attrList != nullptr) {
        result = checkIfSatisfied(policy.get(), attrList.get());
        // std::cout << "Check if satisfied => " << (result.first ? "true" : "false") << endl;
        // std::cout << "Number of matches => " << result.second << endl;
      }
    }
    catch(errors) {}
  }

  return result.first;
}

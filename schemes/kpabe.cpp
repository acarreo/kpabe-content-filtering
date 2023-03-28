#include <algorithm>
#include "kpabe.h"


bool KPABE_DPVS_master_public_key_init(KPABE_DPVS_master_public_key_t mpk)
{
  if (dpvs_init_base_vect(mpk->d1, ND) && dpvs_init_base_vect(mpk->d3, ND) &&
      dpvs_init_base_vect(mpk->g1, NG) && dpvs_init_base_vect(mpk->g2, NG) &&
      dpvs_init_base_vect(mpk->f1, NF) && dpvs_init_base_vect(mpk->f2, NF) &&
      dpvs_init_base_vect(mpk->f3, NF) && dpvs_init_base_vect(mpk->h1, NH) &&
      dpvs_init_base_vect(mpk->h2, NH) && dpvs_init_base_vect(mpk->h3, NH))
  {
    return true;
  }

  return false;
}

bool KPABE_DPVS_master_secret_key_init(KPABE_DPVS_master_secret_key_t msk)
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

/* On ne gere pas le cas d'erreurs (allocation fail) ici, mais il faudra penser à liberer
 * correctement si une allocation echoue
 */
bool KPABE_DPVS_decryption_key_init(KPABE_DPVS_decryption_key_t dec_key, uint size_wl, uint size_bl, uint size_att)
{
  bool ret = false;
  uint i = 0, j = 0, k = 0;

  dec_key->size_wl = size_wl;
  dec_key->size_bl = size_bl;
  dec_key->size_att = size_att;

  if (dpvs_init_dual_base_vect(dec_key->key_root, ND))
  {
    /* Initialize K_WL in the dual base F* */
    if ((dec_key->keys_wl = (g2_vect_st**)malloc(size_wl * sizeof(g2_vect_st*))) != NULL) {
      for (; i < size_wl; i++) {
        if ((dec_key->keys_wl[i] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(dec_key->keys_wl[i], NF)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }

    /* Initialize K_BL in the dual base G* */
    if ((dec_key->keys_bl = (g2_vect_st**)malloc(size_bl * sizeof(g2_vect_st*))) != NULL) {
      for (; j < size_bl; j++) {
        if ((dec_key->keys_bl[j] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(dec_key->keys_bl[j], NG)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }

    /* Initialize K_att for j in list_att(policy) in the dual base H* */
    if ((dec_key->keys_att = (g2_vect_st**)malloc(size_att * sizeof(g2_vect_st*))) != NULL) {
      for (; k < size_att; k++) {
        if ((dec_key->keys_att[k] = (g2_vect_st*)malloc(sizeof(g2_vect_st))) == NULL ||
            !dpvs_init_dual_base_vect(dec_key->keys_att[k], NH)) {
              ret = _error_alloc_fail_();
              break;
        }
      }
    }
  }

  ret = (i == size_wl) && (j == size_bl) && (k == size_att);

  return ret;
}

bool KPABE_DPVS_ciphertext_init(KPABE_DPVS_ciphertext_t cipher, uint size_att)
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

bool KPABE_DPVS_generate_params(KPABE_DPVS_master_public_key_t mpk, KPABE_DPVS_master_secret_key_t msk)
{
  bool ret = true;
  dpvs_t D, G, F, H;

  if (KPABE_DPVS_master_public_key_init(mpk) && KPABE_DPVS_master_secret_key_init(msk)) {
    if (dpvs_gen(D, ND) && dpvs_gen(G, NG) && dpvs_gen(F, NF) && dpvs_gen(H, NH))
    {
      /* Public key */
      dpvs_g1_vect_copy(mpk->d1, D->base[0]);
      dpvs_g1_vect_copy(mpk->d3, D->base[2]);

      dpvs_g1_vect_copy(mpk->g1, G->base[0]);
      dpvs_g1_vect_copy(mpk->g2, G->base[1]);

      dpvs_g1_vect_copy(mpk->f1, F->base[0]);
      dpvs_g1_vect_copy(mpk->f2, F->base[1]);
      dpvs_g1_vect_copy(mpk->f3, F->base[2]);

      dpvs_g1_vect_copy(mpk->h1, H->base[0]);
      dpvs_g1_vect_copy(mpk->h2, H->base[1]);
      dpvs_g1_vect_copy(mpk->h3, H->base[2]);

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

void KPABE_DPVS_master_public_key_destroy(KPABE_DPVS_master_public_key_t mpk)
{
  dpvs_clear_base_vect(mpk->d1); dpvs_clear_base_vect(mpk->d3);
  dpvs_clear_base_vect(mpk->g1); dpvs_clear_base_vect(mpk->g2);
  dpvs_clear_base_vect(mpk->f1); dpvs_clear_base_vect(mpk->f2);
  dpvs_clear_base_vect(mpk->f3); dpvs_clear_base_vect(mpk->h1);
  dpvs_clear_base_vect(mpk->h2); dpvs_clear_base_vect(mpk->h3);
}

void KPABE_DPVS_master_secret_key_destroy(KPABE_DPVS_master_secret_key_t msk)
{
  dpvs_clear_dual_base_vect(msk->d1); dpvs_clear_dual_base_vect(msk->d3);
  dpvs_clear_dual_base_vect(msk->g1); dpvs_clear_dual_base_vect(msk->g2);
  dpvs_clear_dual_base_vect(msk->f1); dpvs_clear_dual_base_vect(msk->f2);
  dpvs_clear_dual_base_vect(msk->f3); dpvs_clear_dual_base_vect(msk->h1);
  dpvs_clear_dual_base_vect(msk->h2); dpvs_clear_dual_base_vect(msk->h3);
}

void KPABE_DPVS_decryption_key_destroy(KPABE_DPVS_decryption_key_t dec_key)
{
  for (uint i = 0; i < dec_key->size_wl; i++) {
    dpvs_clear_dual_base_vect(dec_key->keys_wl[i]);
    free(dec_key->keys_wl[i]);
  }

  for (uint j = 0; j < dec_key->size_bl; j++) {
    dpvs_clear_dual_base_vect(dec_key->keys_bl[j]);
    free(dec_key->keys_bl[j]);
  }

  for (uint k = 0; k < dec_key->size_att; k++) {
    dpvs_clear_dual_base_vect(dec_key->keys_att[k]);
    free(dec_key->keys_att[k]);
  }

  free(dec_key->keys_wl);
  free(dec_key->keys_bl);
  free(dec_key->keys_att);
  dpvs_clear_dual_base_vect(dec_key->key_root);
}

void KPABE_DPVS_ciphertext_destroy(KPABE_DPVS_ciphertext_t cipher)
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
    std::unique_ptr<OpenABEPolicy> policy = createPolicyTree(policy_str);
    std::unique_ptr<OpenABEAttributeList> attrList = createAttributeList(attributes);
    if (policy != nullptr && attrList != nullptr) {
      result = checkIfSatisfied(policy.get(), attrList.get());
    }
  }

  return result.first;
}


bool KPABE_DPVS_generate_decryption_key(KPABE_DPVS_decryption_key_t dec_key,
        KPABE_DPVS_master_secret_key_t msk, std::string& policy_str,
        std::vector<std::string>& wl, std::vector<std::string>& bl)
{
  bn_vect_t ri;
  bn_t y0, y1, tmp1, tmp2;
  g2_vect_t vect_tmp;

  BPGroup group(OpenABE_NONE_ID);
  OpenABELSSS lsss;
  ZP secret_y2;

  uint size_bl = bl.size();
  uint size_wl = wl.size();

  bn_null(tmp1); bn_new(tmp1);
  bn_null(tmp2); bn_new(tmp2);

  bn_null(y1); bn_new(y1); bn_zero(y1); /* y1 := \sum_{i=1}^{|BL|} ri */
  bn_null(y0); bn_new(y0); bn_zero(y0); /* y0 := y1 + secret_y2 */
  bn_null(secret_y2.m_ZP); bn_new(secret_y2.m_ZP);

  std::unique_ptr<OpenABEPolicy> policy = createPolicyTree(policy_str);
  if (policy == nullptr) {
    std::cout << "Errors while trying to create PolicyTree" << std::endl;
    return false;
  }

  bn_vect_init(ri, size_bl);
  for (uint i = 0; i < size_bl; i++) {
    bn_rand_mod(ri->coord[i], group.order);
    bn_mod_add(y1, y1, ri->coord[i], group.order);
  }

  secret_y2.setRandom(group.order);
  bn_mod_add(y0, y1, secret_y2.m_ZP, group.order);

  lsss.shareSecret(policy.get(), secret_y2);
  OpenABELSSSRowMap secret_shares = lsss.getRows();

  if (!KPABE_DPVS_decryption_key_init(dec_key, size_wl, size_bl, secret_shares.size()))
    return false;

  /* set key_root : -y0 * msk->d1 + msk->d3 */
  bn_neg(tmp1, y0); bn_mod(tmp1, tmp1, group.order);
  dpvs_k_mul_dual_vect(dec_key->key_root, msk->d1, tmp1);
  dpvs_add_dual_vect(dec_key->key_root, dec_key->key_root, msk->d3);

  /* set keys whitelist
    (theta[i] * url[i]) * msk->f1 - theta[i] * msk->f2 + y0 * msk->f3 */
  dpvs_init_dual_base_vect(vect_tmp, NF);
  for (uint i = 0; i < size_wl; i++) {
    hash_to_bn(tmp1, wl[i].c_str(), wl[i].size());
    bn_mod(tmp1, tmp1, group.order);
    bn_rand_mod(tmp2, group.order);
    bn_mod_mul(tmp1, tmp1, tmp2, group.order);
    bn_neg(tmp2, tmp2); bn_mod(tmp2, tmp2, group.order);

    dpvs_k_mul_dual_vect(dec_key->keys_wl[i], msk->f1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->f2, tmp2);
    dpvs_add_dual_vect(dec_key->keys_wl[i], dec_key->keys_wl[i], vect_tmp);
    dpvs_k_mul_dual_vect(vect_tmp, msk->f3, y0);
    dpvs_add_dual_vect(dec_key->keys_wl[i], dec_key->keys_wl[i], vect_tmp);
  }
  dpvs_clear_dual_base_vect(vect_tmp);

  /* set keys for blacklist */
  dpvs_init_dual_base_vect(vect_tmp, NG);
  for (uint i = 0; i < size_bl; i++) {
    hash_to_bn(tmp1, bl[i].c_str(), bl[i].size());
    bn_mod(tmp1, tmp1, group.order);
    bn_mod_mul(tmp1, tmp1, ri->coord[i], group.order);
    bn_neg(tmp2, ri->coord[i]); bn_mod(tmp2, tmp2, group.order);

    dpvs_k_mul_dual_vect(dec_key->keys_bl[i], msk->g1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->g2, tmp2);
    dpvs_add_dual_vect(dec_key->keys_bl[i], dec_key->keys_bl[i], vect_tmp);
  }
  dpvs_clear_dual_base_vect(vect_tmp);

  /* set keys k_att */
  dpvs_init_dual_base_vect(vect_tmp, NH);
  int i = 0;
  for(auto it = secret_shares.cbegin(); it != secret_shares.cend(); ++it, i++)
  {
    ZP aj = it->second.element();
    std::string att_j = it->second.label();

    hash_to_bn(tmp1, att_j.c_str(), att_j.size()); /* att_j */
    bn_mod(tmp1, tmp1, group.order);
    bn_rand_mod(tmp2, group.order);                /* theta_j */
    bn_mod_mul(tmp1, tmp1, tmp2, group.order);     /* theta_j * att_j */
    bn_neg(tmp2, tmp2); bn_mod(tmp2, tmp2, group.order); /* - theta_j */

    dpvs_k_mul_dual_vect(dec_key->keys_att[i], msk->h1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->h2, tmp2);
    dpvs_add_dual_vect(dec_key->keys_att[i], dec_key->keys_att[i], vect_tmp);
    dpvs_k_mul_dual_vect(vect_tmp, msk->h3, aj.m_ZP);
    dpvs_add_dual_vect(dec_key->keys_att[i], dec_key->keys_att[i], vect_tmp);
  }
  dpvs_clear_dual_base_vect(vect_tmp);

  bn_free(y0);
  bn_free(y1);
  bn_free(tmp1);
  bn_free(tmp2);
  bn_vect_clear(ri);

  return true;
}

bool KPABE_DPVS_encrypt(KPABE_DPVS_ciphertext_t cipher, bn_t psi,
        KPABE_DPVS_master_public_key_t mpk, std::string& url, std::string& attributes)
{
  bn_t sigma, omega, __url, tmp, sigma_j;
  g1_vect_t vect_tmp, vect_tmp2;

  BPGroup group(OpenABE_NONE_ID);

  std::unique_ptr<OpenABEAttributeList> attrList = createAttributeList(attributes);
  const std::vector<std::string>* attributes_list = attrList->getAttributeList();
  uint size_att = attributes_list->size();

  if (!KPABE_DPVS_ciphertext_init(cipher, size_att))
    return false;

  bn_null(sigma); bn_new(sigma);
  bn_null(omega); bn_new(omega);
  bn_null(__url); bn_new(__url);
  bn_null(sigma_j); bn_new(sigma_j);
  bn_null(tmp); bn_new(tmp);

  bn_rand_mod(sigma, group.order);
  bn_rand_mod(omega, group.order);
  hash_to_bn(__url, url.c_str(), url.size());
  bn_mod(__url, __url, group.order);

  /* ctx root */
  dpvs_init_base_vect(vect_tmp, ND);
  dpvs_k_mul_vect(cipher->ctx_root, mpk->d1, omega);
  dpvs_k_mul_vect(vect_tmp, mpk->d3, psi);
  dpvs_add_vect(cipher->ctx_root, cipher->ctx_root, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  /* whitelist ctx url */
  dpvs_init_base_vect(vect_tmp, NF);
  dpvs_k_mul_vect(cipher->ctx_wl, mpk->f1, sigma);
  bn_mod_mul(tmp, sigma, __url, group.order);
  dpvs_k_mul_vect(vect_tmp, mpk->f2, tmp);
  dpvs_add_vect(cipher->ctx_wl, cipher->ctx_wl, vect_tmp);
  dpvs_k_mul_vect(vect_tmp, mpk->f3, omega);
  dpvs_add_vect(cipher->ctx_wl, cipher->ctx_wl, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  /* blacklist ctx url */
  dpvs_init_base_vect(vect_tmp, NG);
  dpvs_k_mul_vect(cipher->ctx_bl, mpk->g1, omega);
  bn_mod_mul(tmp, omega, __url, group.order);
  dpvs_k_mul_vect(vect_tmp, mpk->g2, tmp);
  dpvs_add_vect(cipher->ctx_bl, cipher->ctx_bl, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  dpvs_init_base_vect(vect_tmp, NH);
  dpvs_init_base_vect(vect_tmp2, NH);
  dpvs_k_mul_vect(vect_tmp2, mpk->h3, omega);
  uint j = 0;
  for (const auto& attr : *attributes_list) {
    bn_rand_mod(sigma_j, group.order);
    dpvs_k_mul_vect(cipher->ctx_att[j], mpk->h1, sigma_j);
    hash_to_bn(tmp, attr.c_str(), attr.size());
    bn_mod(tmp, tmp, group.order);
    bn_mod_mul(tmp, tmp, sigma_j, group.order);
    dpvs_k_mul_vect(vect_tmp, mpk->h2, tmp);
    dpvs_add_vect(cipher->ctx_att[j], cipher->ctx_att[j], vect_tmp);
    dpvs_add_vect(cipher->ctx_att[j], cipher->ctx_att[j], vect_tmp2);
    j++;
  }
  dpvs_clear_base_vect(vect_tmp);
  dpvs_clear_base_vect(vect_tmp2);

  bn_free(tmp);
  bn_free(omega);
  bn_free(__url);
  bn_free(sigma);
  bn_free(sigma_j);

  return true;
}

bool KPABE_DPVS_decrypt(bn_t psi, KPABE_DPVS_ciphertext_t ciphertext, KPABE_DPVS_decryption_key_t dec_key)
{



  return false;
}




{
}

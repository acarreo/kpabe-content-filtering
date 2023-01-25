#include <algorithm>
#include "kpabe.h"

static void hash_to_bn(bn_t hash, const char *digest, int len);

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

/* On ne gere pas le cas d'erreurs (allocation fail) ici, mais il faudra penser à liberer
 * correctement si une allocation echoue
 */
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

void ABE_free_secret_key(ABE_secret_key_t sk)
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

void ABE_free_ciphertext(ABE_cipher_t cipher)
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


bool ABE_key_gen(ABE_secret_key_t sk, ABE_ms_key_t msk, std::string& policy_str,
                 WhiteList_t wl, BlackList_t bl)
{
  bn_vect_t ri;
  bn_t y0, y1, tmp1, tmp2;
  g2_vect_t vect_tmp;

  BPGroup group(OpenABE_NONE_ID);
  OpenABELSSS lsss;
  ZP secret_y2;

  bool ret = false;
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

  if (!ABE_secret_key_init(sk, size_wl, size_bl, secret_shares.size()))
    return false;

  /* set key_root : -y0 * msk->d1 + msk->d3 */
  bn_neg(tmp1, y0); bn_mod(tmp1, tmp1, group.order);
  dpvs_k_mul_dual_vect(sk->key_root, msk->d1, tmp1);
  dpvs_add_dual_vect(sk->key_root, sk->key_root, msk->d3);

  /* set keys whitelist
    (theta[i] * url[i]) * msk->f1 - theta[i] * msk->f2 + y0 * msk->f3 */
  dpvs_init_dual_base_vect(vect_tmp, NF);
  for (uint i = 0; i < size_wl; i++) {
    hash_to_bn(tmp1, wl[i].c_str(), wl[i].size());
    bn_mod(tmp1, tmp1, group.order);
    bn_rand_mod(tmp2, group.order);
    bn_mod_mul(tmp1, tmp1, tmp2, group.order);
    bn_neg(tmp2, tmp2); bn_mod(tmp2, tmp2, group.order);

    dpvs_k_mul_dual_vect(sk->keys_wl[i], msk->f1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->f2, tmp2);
    dpvs_add_dual_vect(sk->keys_wl[i], sk->keys_wl[i], vect_tmp);
    dpvs_k_mul_dual_vect(vect_tmp, msk->f3, y0);
    dpvs_add_dual_vect(sk->keys_wl[i], sk->keys_wl[i], vect_tmp);
  }
  dpvs_clear_dual_base_vect(vect_tmp);

  /* set keys for blacklist */
  dpvs_init_dual_base_vect(vect_tmp, NG);
  for (uint i = 0; i < size_bl; i++) {
    hash_to_bn(tmp1, bl[i].c_str(), bl[i].size());
    bn_mod(tmp1, tmp1, group.order);
    bn_mod_mul(tmp1, tmp1, ri->coord[i], group.order);
    bn_neg(tmp2, ri->coord[i]); bn_mod(tmp2, tmp2, group.order);

    dpvs_k_mul_dual_vect(sk->keys_bl[i], msk->g1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->g2, tmp2);
    dpvs_add_dual_vect(sk->keys_bl[i], sk->keys_bl[i], vect_tmp);
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

    dpvs_k_mul_dual_vect(sk->keys_att[i], msk->h1, tmp1);
    dpvs_k_mul_dual_vect(vect_tmp, msk->h2, tmp2);
    dpvs_add_dual_vect(sk->keys_att[i], sk->keys_att[i], vect_tmp);
    dpvs_k_mul_dual_vect(vect_tmp, msk->h3, aj.m_ZP);
    dpvs_add_dual_vect(sk->keys_att[i], sk->keys_att[i], vect_tmp);
  }
  dpvs_clear_dual_base_vect(vect_tmp);

  bn_free(y0);
  bn_free(y1);
  bn_free(tmp1);
  bn_free(tmp2);
  bn_vect_clear(ri);

  return ret;
}

bool ABE_encrypt(ABE_cipher_t ctx, bn_t psi, ABE_pub_key_t pk,
                 std::string& url, std::string& attributes)
{
  bn_t sigma, omega, __url, tmp, sigma_j;
  g1_vect_t vect_tmp, vect_tmp2;

  BPGroup group(OpenABE_NONE_ID);

  std::unique_ptr<OpenABEAttributeList> attrList = createAttributeList(attributes);
  const std::vector<std::string>* attributes_list = attrList.getAttributeList();
  uint size_att = attributes_list.size();

  if (!ABE_ciphertext_init(ctx, size_att))
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
  dpvs_k_mul_vect(ctx->ctx_root, pk->d1, omega);
  dpvs_k_mul_vect(vect_tmp, pk->d3, psi);
  dpvs_add_vect(ctx->ctx_root, ctx->ctx_root, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  /* whitelist ctx url */
  dpvs_init_base_vect(vect_tmp, NF);
  dpvs_k_mul_vect(ctx->ctx_wl, pk->f1, sigma);
  bn_mod_mul(tmp, sigma, __url, group.order);
  dpvs_k_mul_vect(vect_tmp, pk->f2, tmp);
  dpvs_add_vect(ctx->ctx_wl, ctx->ctx_wl, vect_tmp);
  dpvs_k_mul_vect(vect_tmp, pk->f3, omega);
  dpvs_add_vect(ctx->ctx_wl, ctx->ctx_wl, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  /* blacklist ctx url */
  dpvs_init_base_vect(vect_tmp, NG);
  dpvs_k_mul_vect(ctx->ctx_bl, pk->g1, omega);
  bn_mod_mul(tmp, omega, __url, group.order);
  dpvs_k_mul_vect(vect_tmp, pk->g2, tmp);
  dpvs_add_vect(ctx->ctx_bl, ctx->ctx_bl, vect_tmp);
  dpvs_clear_base_vect(vect_tmp);

  dpvs_init_base_vect(vect_tmp, NH);
  dpvs_init_base_vect(vect_tmp2, NH);
  dpvs_k_mul_vect(vect_tmp2, pk->h3, omega);
  for (uint j = 0; j < size_att; j++) {
    bn_mod(sigma_j, group.order);
    dpvs_k_mul_vect(ctx->ctx_att[j], pk->h1, sigma_j);
    hash_to_bn(tmp, attributes_list[j].c_str(), attributes_list[j].size());
    bn_mod(tmp, tmp, group.order);
    bn_mod_mul(tmp, tmp, sigma_j, group.order);
    dpvs_k_mul_vect(vect_tmp, pk->h2, tmp);
    dpvs_add_vect(ctx->ctx_att[j], ctx->ctx_att[j], vect_tmp);
    dpvs_add_vect(ctx->ctx_att[j], ctx->ctx_att[j], vect_tmp2);
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

bool ABE_decrypt(bn_t psi, ABE_cipher_t ctx, ABE_secret_key_t sk)
{



  return false;
}



/**********************************************************************/

static void hash_to_bn(bn_t hash, const char *digest, int len)
{
  uint8_t h[RLC_MD_LEN];
  md_map(h, (uint8_t*)digest, len);
  bn_read_bin(hash, h, RLC_MD_LEN);
}

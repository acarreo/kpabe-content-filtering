#ifndef _KP_ABE_
#define _KP_ABE_

#include <vector>
#include <string>

#include "../access_structure/zpolicy.h"
#include "../access_structure/zlsss.h"
#include "../access_structure/zattributelist.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../dpvs/dpvs.h"

#define ND  3
#define NG  4
#define NF  6
#define NH  8

typedef struct
{
  g1_vect_t d1, d3;
  g1_vect_t f1, f2, f3;
  g1_vect_t g1, g2;
  g1_vect_t h1, h2, h3;
} ABE_pub_key_st;

typedef struct
{
  g2_vect_t d1, d3;
  g2_vect_t f1, f2, f3;
  g2_vect_t g1, g2;
  g2_vect_t h1, h2, h3;
} ABE_master_key_st;

typedef struct
{
  g2_vect_t key_root;       // D*
  g2_vect_st **keys_wl;     // F*
  g2_vect_st **keys_bl;     // G*
  g2_vect_st **keys_att;    // H*
  uint size_wl;
  uint size_bl;
  uint size_att;
} ABE_secret_key_st;

typedef struct
{
  g1_vect_t ctx_root;   // D
  g1_vect_t ctx_wl;     // F
  g1_vect_t ctx_bl;     // G
  g1_vect_st **ctx_att; // H
  uint size_att;
} ABE_ciphertext_st;

typedef ABE_pub_key_st    ABE_pub_key_t[1];
typedef ABE_master_key_st ABE_ms_key_t[1];
typedef ABE_secret_key_st ABE_secret_key_t[1];
typedef ABE_ciphertext_st ABE_cipher_t[1];

#ifdef __cplusplus
}
#endif

typedef std::string                  url_t;
typedef std::vector<std::string>     WhiteList_t;
typedef std::vector<std::string>     BlackList_t;


bool ABE_pub_key_init(ABE_pub_key_t pk);
bool ABE_ms_key_init(ABE_ms_key_t msk);
bool ABE_gen_params(ABE_pub_key_t pk, ABE_ms_key_t msk);

bool ABE_secret_key_init(ABE_secret_key_t sk, uint size_wl, uint size_bl, uint size_att);
bool ABE_ciphertext_init(ABE_cipher_t cipher, uint size_att);
// bool ABE_extract(ABE_sk_t sk, ABE_ms_key_t msk, (A, WL, BL));
// bool ABE_encrypt(ABE_ctx_t ct, bn_t phi, ABE_pub_key_t pk, (url, Gama));
// bool ABE_decrypt(bn_t psi, ABE_ctx_t ct, sk_(A, WL, BL));

void ABE_free_pub_key(ABE_pub_key_t pk);
void ABE_free_ms_key(ABE_ms_key_t msk);
void ABE_free_secret_key(ABE_secret_key_t sk);
void ABE_free_ciphertext(ABE_cipher_t cipher);

bool checkSatisfyPolicy(std::string& policy_str, std::string& attributes,
                        WhiteList_t wl, BlackList_t bl, std::string& url);

#endif

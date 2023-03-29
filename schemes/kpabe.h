#ifndef __KPABE_DPVS_H__
#define __KPABE_DPVS_H__

#include <vector>
#include <string>
#include <map>

#include "../lsss/zpolicy.h"
#include "../lsss/zlsss.h"
#include "../lsss/zattributelist.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../dpvs/dpvs.h"
#include "../utils.h"

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
} KPABE_DPVS_master_public_key_st;

typedef struct
{
  g2_vect_t d1, d3;
  g2_vect_t f1, f2, f3;
  g2_vect_t g1, g2;
  g2_vect_t h1, h2, h3;
} KPABE_DPVS_master_secret_key_st;

typedef struct
{
  typedef std::map<std::string, g2_vect_st*> key_map_t;
  g2_vect_t key_root;   // D*
  key_map_t key_wl;     // F*
  key_map_t key_bl;     // G*
  key_map_t key_att;    // H*
  std::string policy;
} KPABE_DPVS_decryption_key_st;

typedef struct
{
  typedef std::map<std::string, g1_vect_st*> ctx_map_t;
  g1_vect_t ctx_root;   // D
  g1_vect_t ctx_wl;     // F
  g1_vect_t ctx_bl;     // G
  ctx_map_t ctx_att;    // H
  std::string attributes;
} KPABE_DPVS_ciphertext_st;

typedef KPABE_DPVS_master_public_key_st KPABE_DPVS_master_public_key_t[1];
typedef KPABE_DPVS_master_secret_key_st KPABE_DPVS_master_secret_key_t[1];
typedef KPABE_DPVS_decryption_key_st    KPABE_DPVS_decryption_key_t[1];
typedef KPABE_DPVS_ciphertext_st        KPABE_DPVS_ciphertext_t[1];

#ifdef __cplusplus
}
#endif

bool KPABE_DPVS_master_public_key_init(KPABE_DPVS_master_public_key_t mpk);
bool KPABE_DPVS_master_secret_key_init(KPABE_DPVS_master_secret_key_t msk);
bool KPABE_DPVS_decryption_key_init(KPABE_DPVS_decryption_key_t dec_key);
bool KPABE_DPVS_ciphertext_init(KPABE_DPVS_ciphertext_t ciphertext);

bool KPABE_DPVS_generate_params(KPABE_DPVS_master_public_key_t mpk, KPABE_DPVS_master_secret_key_t msk);

bool KPABE_DPVS_generate_decryption_key(KPABE_DPVS_decryption_key_t dec_key,
                                        KPABE_DPVS_master_secret_key_t msk,
                                        std::string& policy_str,
                                        std::vector<std::string>& white_list,
                                        std::vector<std::string>& black_list);

bool KPABE_DPVS_encrypt(KPABE_DPVS_ciphertext_t ciphertext, gt_t psi,
                        KPABE_DPVS_master_public_key_t mpk,
                        std::string& url, std::string& attributes);

bool KPABE_DPVS_decrypt(gt_t phi, KPABE_DPVS_ciphertext_t ciphertext,
                        KPABE_DPVS_decryption_key_t dec_key, std::string& url);

void KPABE_DPVS_master_public_key_destroy(KPABE_DPVS_master_public_key_t mpk);
void KPABE_DPVS_master_secret_key_destroy(KPABE_DPVS_master_secret_key_t msk);
void KPABE_DPVS_decryption_key_destroy(KPABE_DPVS_decryption_key_t dec_key);
void KPABE_DPVS_ciphertext_destroy(KPABE_DPVS_ciphertext_t ciphertext);

#endif

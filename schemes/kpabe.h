#ifndef _KP_ABE_
#define _KP_ABE_

#include "dpvs.h"

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

typedef ABE_pub_key_st ABE_pub_key_t[1];
typedef ABE_master_key_st ABE_ms_key_t[1];


bool ABE_pub_key_new(ABE_pub_key_t pk);
bool ABE_ms_key_new(ABE_ms_key_t msk);
bool ABE_setup(ABE_pub_key_t pk, ABE_ms_key_t msk);

void ABE_free_pub_key(ABE_pub_key_t pk);
void ABE_free_ms_key(ABE_ms_key_t msk);

#endif
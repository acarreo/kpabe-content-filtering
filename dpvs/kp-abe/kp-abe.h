#ifndef _KP_ABE_
#define _KP_ABE_

#include "dpvs.h"

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




#endif
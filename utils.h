#ifndef __UTILS_H__
#define __UTILS_H__

#include <relic.h>
#include "dpvs/dpvs.h"

typedef struct
{
  g1_t g1;
  g2_t g2;
  gt_t gt;
  bn_t q;
} group_setting_t[1];

// typedef group_setting_st group_setting_t[1];
extern group_setting_t params;

bool init_libraries(void);
void clean_libraries(void);

void hash_to_bn(bn_t hash, const char *digest, int len);
void gt_md_map(uint8_t* hash, gt_t gt);
void print_vect_base(const G1_VS_VECT vect);
void print_vect_dual_base(const G2_VS_VECT dvect);

#endif

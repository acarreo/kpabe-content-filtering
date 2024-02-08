#ifndef __UTILS_H__
#define __UTILS_H__

#include <relic/relic.h>
#include "dpvs/dpvs.h"

bool init_libraries(void);
void clean_libraries(void);

void str_to_bn(bn_t hash, const char *digest, int len, bn_t order);
void gt_md_map(uint8_t* hash, gt_t gt);
void generate_session_key(uint8_t* session_key, bn_t sk);
void print_vect_base(const G1_VS_VECT vect);
void print_vect_dual_base(const G2_VS_VECT dvect);
void print_session_key(uint8_t* session_key);

#endif

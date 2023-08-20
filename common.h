#ifndef __COMMON_H__
#define __COMMON_H__

#include <relic.h>


#define DEBUG 1

#define KEY_SIZE_128    16
#define KEY_SIZE_256    32
#define KEY_SIZE        KEY_SIZE_256

#define G1_SIZE_BIN     RLC_PC_BYTES * 2 + 1
#define G2_SIZE_BIN     RLC_PC_BYTES * 4 + 1

typedef struct {
    uint8_t data[G1_SIZE_BIN];
} g1_bin_t;

typedef struct {
    uint8_t data[G2_SIZE_BIN];
} g2_bin_t;

#endif
#include <stdio.h>
#include <stdlib.h>
#include <msgpack.h>
#include <stdint.h>
#include <assert.h>

#include "dpvs/dpvs.h"
#include "utils.h"
#include "serial/serial.h"

group_setting_t params;
bn_t Fq;

#define G1_SIZE_BIN     (RLC_PC_BYTES * 2 + 1)

void random_vect_g1 (G1_VS_VECT vect) {
  if (vect && vect->coord) {
    for (uint8_t i = 0; i < vect->dim; i++) {
      g1_rand(vect->coord[i]);
    }
  }
}

void print_hexdump(const void *buffer, size_t size) {
  const uint8_t *data = (const uint8_t *)buffer;

  for (size_t i = 0; i < size; i += 16) {
    printf("%08zx: ", i);

    for (size_t j = 0; j < 16; j++) {
      if (i + j < size) {
        printf("%02x ", data[i + j]);
      } else {
        printf("   ");
      }

      if (j == 7) {
        printf(" ");
      }
    }

    printf("| ");

    for (size_t j = 0; j < 16; j++) {
      if (i + j < size) {
        char c = data[i + j];
        if (c >= 32 && c <= 126) {
          printf("%c", c);
        } else {
          printf(".");
        }
      } else {
        printf(" ");
      }
    }

    printf("\n");
  }
}

int main(int argc, char const *argv[])
{
  if (!init_libraries()) {
    printf("Error initializing libraries.\n");
    return 1;
  }

  // test serializing and deserializing a G1 and G2 vectors
  dpvs_t* dpvs = dpvs_generate_bases(2);
  if (dpvs != NULL) {
    G1_VS_VECT* ptr_vect = dpvs->base;
    G2_VS_VECT* ptr_dual_vect = dpvs->dual_base;

    G1_VS_VECT vect = *ptr_vect;
    G2_VS_VECT dual_vect = *ptr_dual_vect;

    uint8_t* buf_vect = NULL;
    uint8_t* buf_dual_vect = NULL;
    int buf_vect_size = 0;
    int buf_dual_vect_size = 0;

    // serialize
    int ret1 = serialize_g1_vector(vect, &buf_vect, &buf_vect_size);
    int ret2 = serialize_g2_vector(dual_vect, &buf_dual_vect, &buf_dual_vect_size);

    if (ret1 == 0 && ret2 == 0) {
      printf("serialization of vectors successful.\n");

      // deserialize
      G1_VS_VECT vect_deserialized = NULL;
      G2_VS_VECT dual_vect_deserialized = NULL;

      int ret11 = deserialize_g1_vector(buf_vect, buf_vect_size, &vect_deserialized);
      int ret22 = deserialize_g2_vector(buf_dual_vect, buf_dual_vect_size, &dual_vect_deserialized);

      if (ret11 == 0 && ret22 == 0) {
        printf("deserialization of vectors successful.\n");
      }
      else {
        printf("Error deserializing vectors.\n");
      }

      // compare
      bool b1 = dpvs_compare_g1_vect(vect, vect_deserialized);
      bool b2 = dpvs_compare_g2_vect(dual_vect, dual_vect_deserialized);

      if (b1 && b2) {
        printf("Serialization and deserialization of vectors successful.\n");
      }
      else {
        printf("Error comparing vectors.\n");
      }

      // free
      dpvs_clear_g1_vect(vect_deserialized);
      dpvs_clear_g2_vect(dual_vect_deserialized);
    }
    else {
      printf("Error serializing vectors.\n");
    }

    // free
    free(buf_vect);
    free(buf_dual_vect);
  }

  dpvs_clear(dpvs);
  clean_libraries();

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <msgpack.h>
#include <stdint.h>
#include <assert.h>

#include "../dpvs/dpvs.h"
#include "../utils.h"

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

// compare two g1_vect_t
bool compare_g1_vect(const G1_VS_VECT vect1, const G1_VS_VECT vect2) {
  if (vect1->dim != vect2->dim) {
    return false;
  }

  for (uint8_t i = 0; i < vect1->dim; i++) {
    if (g1_cmp(vect1->coord[i], vect2->coord[i]) != RLC_EQ) {
      return false;
    }
  }

  return true;
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

void serialize_g1_vector(const G1_VS_VECT vect, uint8_t** buf, int* size) {

  if (vect && vect->coord) {
    int shift = 0;
    uint8_t *buf_tmp = (uint8_t*) malloc((vect->dim) * (G1_SIZE_BIN + sizeof(uint8_t)));

    if (buf_tmp == NULL) {
      fprintf(stderr, "Error in serialize_g1_vector: malloc failed\n");
      exit(EXIT_FAILURE);
    }

    for (uint8_t i = 0; i < vect->dim; i++) {
      uint8_t g1_size = g1_size_bin(vect->coord[i], 0);
      memcpy(buf_tmp + shift, &g1_size, sizeof(uint8_t));
      g1_write_bin(buf_tmp + shift + sizeof(uint8_t), g1_size, vect->coord[i], 0);
      shift += g1_size + sizeof(uint8_t);
    }

    *size = sizeof(int) + sizeof(uint8_t) + shift;
    *buf = (uint8_t*) malloc(*size);
    if (buf == NULL) {
      fprintf(stderr, "Error in serialize_g1_vector: malloc failed\n");
      free(buf_tmp);
      exit(EXIT_FAILURE);
    }

    memcpy(*buf, &shift, sizeof(int));   // write size of buffer representing the g1 vector
    memcpy(*buf + sizeof(int), &vect->dim, sizeof(uint8_t));  // write dimension of g1 vector
    memcpy(*buf + sizeof(int) + sizeof(uint8_t), buf_tmp, shift);  // write g1 vector

    free(buf_tmp);
  }
}

void deserialize_g1_vector(const uint8_t* buf, const int size, g1_vect_t** deserialized_vector) {
  int buf_size = 0;
  int shift = 0;
  uint8_t dim = 0;
  if (buf != NULL && memcpy(&buf_size, buf, sizeof(int)) && buf_size > 0) {
    memcpy(&dim, buf + sizeof(int), sizeof(uint8_t));
    shift = sizeof(int) + sizeof(uint8_t);

    *deserialized_vector = dpvs_create_g1_vect(dim);
    G1_VS_VECT vect = *deserialized_vector;
    if (vect != NULL) {
      for (uint8_t i = 0; i < dim; i++) {
        uint8_t g1_size = 0;
        memcpy(&g1_size, buf + shift, sizeof(uint8_t));
        g1_read_bin(vect->coord[i], buf + shift + sizeof(uint8_t), g1_size);
        shift += g1_size + sizeof(uint8_t);
      }

      if (shift != size || (shift - sizeof(int) - sizeof(uint8_t)) != buf_size) {
        fprintf(stderr, "Error deserializing the g1 vector: the size of the buffer is not correct.\n");
        dpvs_clear_g1_vect(vect);
        return;
      }
    }
    else {
      fprintf(stderr, "Error deserializing the g1 vector: dpvs_create_g1_vect failed.\n");
      return;
    }
  }
}


int main(int argc, char const *argv[])
{
  if (!init_libraries()) {
    printf("Error initializing libraries.\n");
    return 1;
  }

  // test serialization of a G1_VS_VECT
  G1_VS_VECT vect = dpvs_create_g1_vect(5);
  if (!vect) {
    printf("Error creating the G1_VS_VECT.\n");
    clean_libraries();
    return 1;
  }

  random_vect_g1(vect);

  uint8_t *buf = NULL;
  int size = 0;
  serialize_g1_vector(vect, &buf, &size);
  print_hexdump(buf, size);

  G1_VS_VECT vect2;
  deserialize_g1_vector(buf, size, &vect2);


  if (!vect2) {
    printf("Error deserializing the G1_VS_VECT.\n");
    dpvs_clear_g1_vect(vect);
    clean_libraries();
    return 1;
  }

  // check that the two vectors are equal
  if (compare_g1_vect(vect, vect2) == true) {
    printf("Serialization and deserialization of a G1_VS_VECT successful.\n");
    printf("Size of the serialized buffer: %d\n", size);
  } else {
    printf("Serialization and deserialization of a G1_VS_VECT failed.\n");
  }

  // clean vectors
  dpvs_clear_g1_vect(vect);
  dpvs_clear_g1_vect(vect2);
  clean_libraries();

  return 0;
}

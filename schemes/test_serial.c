#include <stdio.h>
#include <stdlib.h>
#include <msgpack.h>
#include <stdint.h>

#include "../dpvs/dpvs.h"
#include "../utils.h"

group_setting_t params;
bn_t Fq;


void random_vect_g1 (G1_VS_VECT vect) {
  if (vect && vect->coord) {
    for (uint8_t i = 0; i < vect->dim; i++) {
      g1_rand(vect->coord[i]);
    }
  }
}

void print_serialized_buffer(const msgpack_sbuffer* sbuf) {
  for (size_t i = 0; i < sbuf->size; i++) {
    printf("%02X",sbuf->data[i] & 0xFF);
  }
  printf("\n");
}

void serialize_g1_vect(const G1_VS_VECT vect, msgpack_sbuffer* sbuf) {
  msgpack_packer packer;
  msgpack_packer_init(&packer, sbuf, msgpack_sbuffer_write);

  msgpack_pack_array(&packer, 2);
  msgpack_pack_uint8(&packer, vect->dim);

  msgpack_pack_array(&packer, vect->dim);
  for (int i = 0; i < vect->dim; i++) {
    int size = g1_size_bin(vect->coord[i], 0);
    uint8_t* buf_g1 = (uint8_t*) malloc(size);

    g1_write_bin(buf_g1, size, vect->coord[i], 0);
    msgpack_pack_v4raw(&packer, size);
    msgpack_pack_v4raw_body(&packer, buf_g1, size);

    free(buf_g1);
  }
}

// deserialize a G1_VS_VECT from a msgpack_sbuffer
#if 0
G1_VS_VECT deserialize_g1_vect(const msgpack_sbuffer* sbuf) {
  G1_VS_VECT vect = NULL;

  msgpack_unpacker unpacker;
  msgpack_unpacker_init(&unpacker, MSGPACK_UNPACKER_INIT_BUFFER_SIZE);

  // Feed the serialized buffer to the unpacker
  msgpack_unpacker_reserve_buffer(&unpacker, sbuf->size);
  memcpy(msgpack_unpacker_buffer(&unpacker), sbuf->data, sbuf->size);
  msgpack_unpacker_buffer_consumed(&unpacker, sbuf->size);

  // Unpack the serialized data
  msgpack_unpacked deserialized_obj;
  msgpack_unpack_return ret = msgpack_unpacker_next(&unpacker, &deserialized_obj, NULL);



  // G1_VS_VECT vect = dpvs_create_g1_vect(dim);
  // if (!vect) {
  //   fprintf(stderr, "Error unpacking the serialized buffer: unable to create the G1_VS_VECT.\n");
  //   return NULL;
  // }

  // for (int i = 0; i < dim; i++) {
  //   deserialized_obj = msgpack_unpack_next(&unpacker, &deserialized_obj);
  //   if (deserialized_obj.type != MSGPACK_OBJECT_RAW) {
  //     fprintf(stderr, "Error unpacking the serialized buffer: the deserialized object is not a raw object.\n");
  //     dpvs_clear_g1_vect(vect);
  //     return NULL;
  //   }
  //   g1_read_bin(vect->coord[i], deserialized_obj.via.raw.ptr, deserialized_obj.via.raw.size);
  // }

  return vect;
}
#endif


int main(int argc, char const *argv[])
{
  if (!init_libraries()) {
    printf("Error initializing libraries.\n");
    return 1;
  }

  G1_VS_VECT vect = dpvs_create_g1_vect(2);
  random_vect_g1(vect);
  print_vect_base(vect);

  // msgpack_sbuffer sbuf;
  // msgpack_sbuffer_init(&sbuf);

  // serialize_g1_vect(vect, &sbuf);
  // print_serialized_buffer(&sbuf);

  // G1_VS_VECT vect_deserialized = deserialize_g1_vect(&sbuf);

  // print_vect_base(vect_deserialized);

  // msgpack_sbuffer_destroy(&sbuf);
  dpvs_clear_g1_vect(vect);
  // dpvs_clear_g1_vect(vect_deserialized);
  clean_libraries();

  return 0;
}

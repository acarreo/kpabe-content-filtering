#include "utils.h"

bool init_libraries(void) {
  if (core_init() == RLC_OK && pc_param_set_any() == RLC_OK) {
    bn_null(Fq); bn_new(Fq);
    pc_get_ord(Fq);
    return true;
  }

  core_clean();
  return false;
}

void str_to_bn(bn_t hash, const char *digest, int len, bn_t order) {
  uint8_t h[RLC_MD_LEN];
  md_map(h, (uint8_t*)digest, len);
  bn_read_bin(hash, h, RLC_MD_LEN);
  bn_mod(hash, hash, order);
}

void gt_md_map(uint8_t* hash, gt_t gt) {
  int l = gt_size_bin(gt, 0);
  uint8_t buf[l];
  gt_write_bin(buf, l, gt, 0);
  md_map(hash, buf, l);
}

void clean_libraries(void) {
  bn_free(Fq);
  core_clean();
}

void print_vect_base(const G1_VS_VECT vect) {
  for (uint8_t i = 0; i < vect->dim; i++) {
    g1_print(vect->coord[i]);
    printf("\n");
  }
}

void print_vect_dual_base(const G2_VS_VECT dvect) {
  for (uint8_t i = 0; i < dvect->dim; i++) {
    g2_print(dvect->coord[i]);
    printf("\n");
  }
}

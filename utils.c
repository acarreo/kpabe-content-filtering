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

void generate_session_key(uint8_t* session_key, bn_t sk) {
  g1_t g1; g1_null(g1); g1_new(g1);
  g2_t g2; g2_null(g2); g2_new(g2);
  gt_t gt; gt_null(gt); gt_new(gt);

  g1_get_gen(g1); g2_get_gen(g2); pc_map(gt, g1, g2);
  bn_null(sk); bn_new(sk); bn_rand_mod(sk, Fq);
  gt_exp(gt, gt, sk);

  gt_md_map(session_key, gt);

  g1_free(g1); g2_free(g2); gt_free(gt);
}

void clean_libraries(void) {
  bn_free(Fq);
  core_clean();
}

void print_vect_base(const g1_vector_ptr vect) {
  for (uint8_t i = 0; i < vect->dim; i++) {
    g1_print(vect->elements[i]);
    printf("\n");
  }
}

void print_vect_dual_base(const g2_vector_ptr dvect) {
  for (uint8_t i = 0; i < dvect->dim; i++) {
    g2_print(dvect->elements[i]);
    printf("\n");
  }
}

void print_session_key(uint8_t* session_key) {
  for (int i = 0; i < RLC_MD_LEN; i++) {
    printf("%02X", session_key[i]);
  }
  printf("\n");
}

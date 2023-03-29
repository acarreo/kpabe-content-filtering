#include "utils.h"

bool init_libraries(void) {
  if (core_init() == RLC_OK && pc_param_set_any() == RLC_OK) {
    set_params();
    return true;
  }

  core_clean();
  return false;
}

/*****************************************************************************
 * the bilinear group setting is (G1, G2, Gt, g1, g2, gt, e, q) where        *
 * G1, G2, Gt are groups of prime order q, g1 \in G1 and g2 \in G2           *
 * e : G1 x G2 --> Gt a pairing and gt = e(g1, g2).                          *
 * g1 (resp. g2) generator of G1 (resp. G2).                                 *
 ****************************************************************************/
void set_params(void) {
  g1_null(params->g1); g1_new(params->g1);
  g2_null(paramq->g2); g2_new(params->g2);

  bn_null(params->q);  bn_new(params->q);
  gt_null(params->gt); gt_new(params->gt);

  g1_get_gen(params->g1);
  g2_get_gen(params->g2);

  pc_map(params->gt, params->g1, params->g2);
  pc_get_ord(params->q);

  bn_copy(Fq, params->q);
}

void hash_to_bn(bn_t hash, const char *digest, int len) {
  uint8_t h[RLC_MD_LEN];
  md_map(h, (uint8_t*)digest, len);
  bn_read_bin(hash, h, RLC_MD_LEN);
}

void gt_md_map(uint8_t* hash, gt_t gt) {
  int l = gt_size_bin(gt, 0);
  uint8_t buf[l];
  gt_write_bin(buf, l, gt, 0);
  md_map(hash, buf, l);
}

void clear_params(void) {
  bn_free(params->q);
  g1_free(params->g1);
  g2_free(params->g2);
  gt_free(params->gt);
  bn_free(Fq);
}

void clean_libraries(void) {
  clear_params();
  core_clean();
}

void print_vect_base(const g1_vect_t vect) {
  for (uint8_t i = 0; i < vect->dim; i++) {
    g1_print(vect->coord[i]);
  }
}

void print_vect_dual_base(const g2_vect_t dvect) {
  for (uint8_t i = 0; i < dvect->dim; i++) {
    g2_print(dvect->coord[i]);
  }
}

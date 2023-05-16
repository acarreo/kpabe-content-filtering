#include <vector>
#include <string>
#include <cassert>
#include <cmocka.h>

#include "schemes/kpabe.h"

group_setting_t params;
bn_t Fq;

static void test_DPVS_dual_bases(void **state)
{
  bool is_dual_pair;
  int dim = 5;
  dpvs_t dpvs;
  gt_t ip;

  gt_null(ip);
  gt_new(ip);

  if ((is_dual_pair = dpvs_gen(dpvs, dim))) {
    for (uint8_t i = 0; i < dim && is_dual_pair; i++) {
      for (uint8_t j = 0; j < dim && is_dual_pair; j++) {
        dpvs_inner_product(ip, dpvs->base[i], dpvs->dual_base[j]);
        is_dual_pair &= (i == j) ? (gt_cmp(ip, params->gt) == RLC_EQ) : gt_is_unity(ip);
      }
    }
  }

  dpvs_clear(dpvs);
  gt_free(ip);

  assert_true(is_dual_pair);
}

static void test_KPABE_DPVS_scheme(void **state)
{
  std::vector<std::string> wl({"hi", "salut", "gentil"});
  std::vector<std::string> bl({"mechant", "uggly", "devil"});
  std::string policy_str("(A1 and A2) or (A3 and A4) or (A5 and A6)");
  std::string attributes("A1|A2|A3|A4");
  std::string url("salut");

  KPABE_DPVS_master_public_key_t pk;
  KPABE_DPVS_master_secret_key_t msk;
  KPABE_DPVS_decryption_key_t sk;
  KPABE_DPVS_ciphertext_t ctx;

  gt_t psi, xi;

  gt_null(psi); gt_new(psi);
  gt_null(xi); gt_new(xi);

  assert_true(KPABE_DPVS_generate_params(pk, msk));
  assert_true(KPABE_DPVS_encrypt(ctx, psi, pk, url, attributes));
  KPABE_DPVS_master_public_key_destroy(pk);

  assert_true(KPABE_DPVS_generate_decryption_key(sk, msk, policy_str, wl, bl));
  KPABE_DPVS_master_secret_key_destroy(msk);

  assert_true(KPABE_DPVS_decrypt(xi, ctx, sk, url));
  KPABE_DPVS_decryption_key_destroy(sk);
  KPABE_DPVS_ciphertext_destroy(ctx);

  /* Final assert */
  assert_true((gt_cmp(psi, xi) == RLC_EQ));

  gt_free(psi);
  gt_free(xi);
}

int main(int argc, char const * argv[])
{
  if (!init_libraries()) return -1;

  /* Initialize the cmocka testing framework */
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_DPVS_dual_bases),
    cmocka_unit_test(test_KPABE_DPVS_scheme)
  };

  /* Run all tests */
  int ret = cmocka_run_group_tests(tests, NULL, NULL);

  clean_libraries();

  return ret;
}

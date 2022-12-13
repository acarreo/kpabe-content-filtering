/*****************************************************************************
 *     gcc *.c -g -Wall -lgmp /usr/lib/librelic_s.a -I/usr/include/relic     *
 *****************************************************************************/

#include "kp-abe.h"

/* Declaration of external variables, there are declared in the headers files */
group_setting_t params;
bn_t Fq;


int main(int argc, char const *argv[])
{
  core_init();
  pc_param_set_any();
  set_params();

  ABE_pub_key_t pk;
  ABE_ms_key_t msk;

  ABE_setup(pk, msk);

  ABE_free_pub_key(pk);
  ABE_free_ms_key(msk);

  clear_params();
  core_clean();

  return 0;
}

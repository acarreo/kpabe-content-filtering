/*****************************************************************************
 *     gcc *.c -g -Wall -lgmp /usr/lib/librelic_s.a -I/usr/include/relic     *
 *****************************************************************************/

#include <assert.h>
#include "test.h"

/* Declaration of external variables, there are declared in the headers files */
group_setting_t params;
bn_t Fq;


int main(int argc, char const *argv[])
{
  int dim;

  if (argc != 2 || (dim = atoi(argv[1])) < 0) {
    fprintf(stderr, "Please enter the dimension (positive integer) of matrix\n");
    return -1;
  }

  core_init();
  pc_param_set_any();
  set_params();

  printf("[*] Cheking DPVS_dual_bases...\n");
  assert(test_DPVS_dual_bases(dim) == true);

  clear_params();
  core_clean();

  return 0;
}

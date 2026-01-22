#include <relic/relic.h>
#include "utils.h"

bn_t Fq;

bool init_libraries(void) {
  if (core_init() == RLC_OK && pc_param_set_any() == RLC_OK) {
    bn_null(Fq); bn_new(Fq);
    pc_get_ord(Fq);
    return true;
  }

  core_clean();
  return false;
}

void clean_libraries(void) {
  bn_free(Fq);
  core_clean();
}

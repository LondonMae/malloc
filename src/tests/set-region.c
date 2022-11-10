#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test uses the environment variable option for setting the region size.
 *
 * Validates that setting the region size allows for a different number of
 * allocations per region.
 */

int main(int argc, char **argv) {
  setenv(REGION_SIZE_ENV_VAR, "32768", 1); // 32k
  init_memory_tracking();

  void *ptrs[20]; // allocate 20k of memory, this should easily fit.
  for (int i = 0; i < 20; i++) {
    ptrs[i] = lynx_malloc(1024);
  }

  checkpoint_memory();

  for (int i = 0; i < 20; i++) {
    lynx_free(ptrs[i]);
  }

  struct tracked_memory t = tracked_memory();

  // 20k should it in 1 region.
  EXPECT_EQ(1, t.regions[1], "large region should accomodate 20k of blocks");

  print_lynx_alloc_debug_info();

  return 0;
}

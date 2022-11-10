#include <stdio.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * Tests that free space is split and that 2000 bytes of allocations can fit in
 * a region.
 */

int main(int argc, char **argv) {
  static void *ptrs[] = {NULL, NULL, NULL, NULL};
  struct tracked_memory t;

  init_memory_tracking();

  // allocate 2k -- should be less than a page
  ptrs[0] = lynx_malloc(500);
  ptrs[1] = lynx_malloc(1000);
  ptrs[2] = lynx_malloc(250);
  ptrs[3] = lynx_malloc(250);

  (void)ptrs[0]; // swallow unused variable warning

  checkpoint_memory();

  t = tracked_memory();
  print_lynx_alloc_debug_info();

  EXPECT_EQ(1, t.regions[1], "500-1000-250-250 should fit in one region");

  return 0;
}

#include <stdio.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * Test that allocates enough to fill most of a region then frees one large
 * block. Proceeds to allocate several small blocks, validating that free blocks
 * are coalesced.
 */

int main(int argc, char **argv) {
  static void *ptrs[] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  struct tracked_memory t;

  init_memory_tracking();

  // allocate 2k -- should be less than a page
  ptrs[0] = lynx_malloc(500);
  ptrs[1] = lynx_malloc(1000);
  ptrs[2] = lynx_malloc(250);
  ptrs[3] = lynx_malloc(250);

  checkpoint_memory();

  // free a 1000-byte block
  lynx_free(ptrs[1]);

  // allocate less than 1k
  ptrs[4] = lynx_malloc(600);
  ptrs[5] = lynx_malloc(150);
  ptrs[6] = lynx_malloc(50);

  checkpoint_memory();

  t = tracked_memory();
  print_lynx_alloc_debug_info();

  EXPECT_LT(t.regions[0], t.regions[1], "malloc must increase region count")
  EXPECT_EQ(t.regions[1], t.regions[2],
            "650-150-50 should fit in a 1000-byte block");

  return 0;
}

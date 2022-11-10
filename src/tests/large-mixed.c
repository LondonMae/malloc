#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test creates a mix of large and small blocks and validates that when
 * small blocks are created, the number of large blocks does not increase.
 */

int main(int argc, char **argv) {
  init_memory_tracking();

  void *ptrs[6];
  ptrs[0] = lynx_malloc(2 * DEFAULT_REGION_SIZE);
  ptrs[1] = lynx_malloc(100);
  ptrs[2] = lynx_malloc(100);
  ptrs[3] = lynx_malloc(100);
  ptrs[4] = lynx_malloc(100);
  ptrs[5] = lynx_malloc(2 * DEFAULT_REGION_SIZE);

  checkpoint_memory();

  for (int i = 0; i < 6; i++) {
    lynx_free(ptrs[i]);
  }

  struct tracked_memory t = tracked_memory();
  EXPECT_EQ(2, t.large_blocks[1], "only 2 large blocks allocated");

  print_lynx_alloc_debug_info();
  return 0;
}

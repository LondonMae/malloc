#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test uses the environment variable option for setting large block size.
 *
 * Validates that allocating a blocks that would otherwise fit in a region
 * creates a large block for each.
 */

int main(int argc, char **argv) {
  setenv(REGION_SIZE_ENV_VAR, "32768", 1);   // regions are 32k
  setenv(MAX_BLOCK_ALLOC_ENV_VAR, "512", 1); // but large blocks are small
  init_memory_tracking();

  void *ptrs[20]; // allocate 10k of memory, but they should all be large blocks
  for (int i = 0; i < 20; i++) {
    ptrs[i] = lynx_malloc(513);
  }

  checkpoint_memory();

  for (int i = 0; i < 20; i++) {
    lynx_free(ptrs[i]);
  }

  struct tracked_memory t = tracked_memory();

  // all blocks should be large
  EXPECT_EQ(20, t.large_blocks[1],
            "large blocks should be defined by the threshold");

  print_lynx_alloc_debug_info();

  return 0;
}

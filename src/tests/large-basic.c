#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test allocates large blocks and validates that blocks larger than the
 * region size can be created and are freed when free is called.
 */

int main(int argc, char **argv) {
  init_memory_tracking();

  int cp_init, cp_peak, cp_final;
  cp_init = 0;

  // allocate blocks that should be larger than a region
  void *a = lynx_malloc(2 * MAX_BLOCK_ALLOC);
  checkpoint_memory();
  void *b = lynx_malloc(2 * DEFAULT_REGION_SIZE);
  checkpoint_memory();
  void *c = lynx_malloc(100 * (1 << 20)); // 100MB
  checkpoint_memory();
  cp_peak = 3;

  // free them
  lynx_free(a);
  checkpoint_memory();
  lynx_free(b);
  checkpoint_memory();
  lynx_free(c);
  checkpoint_memory();
  cp_final = 6;

  // validate that large blocks and pages increase and decrease
  struct tracked_memory t = tracked_memory();
  EXPECT_EQ(t.pages[cp_init], t.pages[cp_final],
            "final should be equal to start");
  EXPECT_EQ(0, t.large_blocks[cp_init], "no initial large blocks");
  EXPECT_EQ(3, t.large_blocks[cp_peak], "three total large blocks");
  EXPECT_EQ(0, t.large_blocks[cp_final], "no final large blocks");

  for (int i = 0; i < cp_peak; i++) {
    EXPECT_LT(t.large_blocks[i], t.large_blocks[i + 1],
              "blocks should be increasing");
    EXPECT_LT(t.pages[i], t.pages[i + 1], "memory usage should be increasing");
  }
  for (int i = cp_peak; i < cp_final; i++) {
    EXPECT_GT(t.large_blocks[i], t.large_blocks[i + 1],
              "blocks should be decreasing");
    EXPECT_GT(t.pages[i], t.pages[i + 1], "memory usage should be decreasing");
  }

  print_lynx_alloc_debug_info();

  return 0;
}

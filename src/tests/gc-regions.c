#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test creates a large amount of allocations and then subsequently frees
 * them. The test verifies that there aren't a large number of free regions left
 * at the end (i.e., that regions are garbage collected when they are no longer
 * used.
 */

#define N_PTRS 1024
#define N_ITERS 4
#define BLOCK_SIZE 100

int main(int argc, char **argv) {
  init_memory_tracking();

  uint64_t page_limit = 2;

  void *ptrs[N_PTRS];

  for (int i = 0; i < N_ITERS; i++) {
    // allocate memory
    for (int p = 0; p < N_PTRS; p++) {
      ptrs[p] = lynx_malloc(BLOCK_SIZE);
    }
    // free half; then allocate larger
    for (int p = 0; p < N_PTRS; p += 2) {
      lynx_free(ptrs[p]);
      ptrs[p] = lynx_malloc(2 * BLOCK_SIZE);
    }
    // free all
    for (int p = 0; p < N_PTRS; p++) {
      lynx_free(ptrs[p]);
    }
  }

  checkpoint_memory();

  struct tracked_memory t = tracked_memory();
  EXPECT_LT(t.pages[1] - t.pages[0], page_limit, "too many pages!");
  EXPECT_LT(t.regions[1] - t.regions[0], page_limit, "too many regions!");

  print_lynx_alloc_debug_info();

  return 0;
}

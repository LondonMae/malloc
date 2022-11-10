#include <stdlib.h>
#include <string.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates that large blocks can be used with realloc:
 * - we can create a large block and reallocate to another large block.
 * - we can create a large block and reallocate to a small block.
 * - we can create a small block and reallocate to a large block.
 */

int main(int argc, char **argv) {
  init_memory_tracking();

  void *arr = NULL;
  // large -> larger -> small -> larger
  size_t sizes[4] = {2 * DEFAULT_REGION_SIZE, 4 * DEFAULT_REGION_SIZE, 100,
                     4 * DEFAULT_REGION_SIZE};
  char fill[4] = {0xaa, 0x55, 0xaa, 0x55};

  // on the first allocation, we should go from NULL to sizes[0] elements.
  arr = lynx_realloc(arr, sizes[0]);
  memset(arr, fill[0], sizes[0]);

  checkpoint_memory();

  for (int i = 1; i < 4; i++) {
    arr = lynx_realloc(arr, sizes[i]);
    // verify that the previous values were carried over
    if (sizes[i] > 0) {
      verify_contents(arr, fill[i - 1], MIN(sizes[i - 1], sizes[i]));
      // fill with new values
      memset(arr, fill[i], sizes[i]);
    }
    checkpoint_memory();
  }

  lynx_free(arr);

  checkpoint_memory();
  struct tracked_memory t = tracked_memory();
  for (int i = 0; i < 6; i++) {
    printf("%d: %lu\n", i, t.large_blocks[i]);
  }

  // validate the number of large blocks
  EXPECT_EQ(0, t.large_blocks[0], "initially no large blocks");
  EXPECT_EQ(1, t.large_blocks[1], "NULL -> large");
  EXPECT_EQ(1, t.large_blocks[2], "large -> larger");
  EXPECT_EQ(0, t.large_blocks[3], "larger -> small");
  EXPECT_EQ(1, t.large_blocks[4], "small -> large");
  EXPECT_EQ(0, t.large_blocks[5], "large -> free");

  print_lynx_alloc_debug_info();

  return 0;
}

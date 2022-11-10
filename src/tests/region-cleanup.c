#include <stdio.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * Test that allocates enough 1000-byte blocks to be more than one region. The
 * test then frees almost all of these blocks, and therefore uses less than one
 * region.
 *
 * This test validates that regions (and therefore pages) are cleaned up.
 *
 * This test will pass without splitting/coalescing, since region counts will
 * increase/decrease appropriately.
 */

int main(int argc, char **argv) {
  static void *ptrs[] = {NULL, NULL, NULL, NULL, NULL, NULL};

  init_memory_tracking();

  // allocate 6k -- should be 2 pages
  ptrs[0] = lynx_malloc(1000);
  ptrs[1] = lynx_malloc(1000);
  ptrs[2] = lynx_malloc(1000);
  ptrs[3] = lynx_malloc(1000);
  ptrs[4] = lynx_malloc(1000);
  ptrs[5] = lynx_malloc(1000);

  checkpoint_memory();

  // free more than a page
  lynx_free(ptrs[5]);
  lynx_free(ptrs[4]);
  lynx_free(ptrs[3]);
  lynx_free(ptrs[2]);
  lynx_free(ptrs[1]);

  // allocate less than the free space in one page
  ptrs[1] = lynx_malloc(10);
  ptrs[2] = lynx_malloc(10);
  ptrs[3] = lynx_malloc(10);
  ptrs[4] = lynx_malloc(10);
  ptrs[5] = lynx_malloc(10);

  checkpoint_memory();

  // At this point, an allocator that collects free regions/pages and unmamps
  // them should have decreased the number of pages.
  struct tracked_memory t = tracked_memory();
  printf("used pages: %lu - %lu - %lu\n", t.pages[0], t.pages[1], t.pages[2]);
  printf("alloc regions: %lu - %lu - %lu\n", t.regions[0], t.regions[1],
         t.regions[2]);
  print_lynx_alloc_debug_info();

  EXPECT_LT(t.pages[0], t.pages[1], "Expect pages allocated after malloc");
  EXPECT_LT(t.pages[2], t.pages[1], "Expect pages to be cleaned up after free");
  EXPECT_LT(t.regions[0], t.regions[1],
            "Expect regions allocated after malloc");
  EXPECT_LT(t.regions[2], t.regions[1],
            "Expect regions to be cleaned up after free");

  return 0;
}

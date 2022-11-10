#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * Basic test that allocates a series of 100 byte blocks along with a smaller
 * block. The test simply validates that all pointers are distinct, but it can
 * be used to test a fit strategy by examining the debug output.
 */

int main(int argc, char **argv) {
  void *ptrs[] = {NULL, NULL, NULL, NULL, NULL, NULL};

  ptrs[0] = lynx_malloc(100);
  ptrs[1] = lynx_malloc(100); // later freed
  ptrs[2] = lynx_malloc(100);
  ptrs[3] = lynx_malloc(10); // later freed
  ptrs[4] = lynx_malloc(100);
  ptrs[5] = lynx_malloc(100);

  lynx_free(ptrs[1]);
  lynx_free(ptrs[3]);

  // first fit: will fill space freed by ptrs[1]
  // best fit: will fill space freed by ptrs[3]
  // worst fit: will fill space after ptrs[5]
  void *fit = lynx_malloc(10);
  (void)fit; // swallow unused warning

  // you should be able to manually verify the fit scheme.
  print_lynx_alloc_debug_info();

  EXPECT_NEQ(ptrs[0], ptrs[2], "duplicate pointer returned");
  EXPECT_NEQ(ptrs[2], ptrs[4], "duplicate pointer returned");
  EXPECT_NEQ(ptrs[4], ptrs[5], "duplicate pointer returned");
  EXPECT_NEQ(fit, ptrs[0], "duplicate pointer returned");

  return 0;
}

#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates that large blocks that are allocated with calloc are
 * zeroed.
 */

int main(int argc, char **argv) {
  void *a = lynx_calloc(MAX_BLOCK_ALLOC, 4);
  verify_empty(a, 4 * MAX_BLOCK_ALLOC);
  print_lynx_alloc_debug_info();
  return 0;
}

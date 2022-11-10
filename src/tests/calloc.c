#include <stdlib.h>
#include <string.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates that calloc zeros the requested memory. We allocate a
 * block of memory, scribble it, free it, and then allocate the same amount with
 * calloc.
 */

int main(int argc, char **argv) {
  void *p = lynx_calloc(50, 4); // 50*4 = 200 bytes
  verify_empty(p, 200);

  // scribble the memory and then free it -- the block should be re-used.
  memset(p, 0xff, 200);
  lynx_free(p);

  p = lynx_calloc(25, 8); // 25*8 = 200 bytes
  verify_empty(p, 200);

  print_lynx_alloc_debug_info();

  return 0;
}

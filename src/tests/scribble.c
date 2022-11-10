#include <stdlib.h>
#include <string.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates that if the scribble character has been set, memory is
 * scribbled. It additionally validates that scribbling with realloc does not
 * overwrite the existing data and that calloc's memory is still zeroed.
 */

int main(int argc, char **argv) {
  // set environment variables before initializing lynx allocator
  setenv(SCRIBBLE_ENV_VAR, "0xb5", 1); // 10100101

  init_memory_tracking();

  void *a = lynx_malloc(100);
  verify_contents(a, 0xb5, 100);

  // invert bits
  memset(a, 0x5b, 100);

  void *b = lynx_malloc(50);
  verify_contents(b, 0xb5, 50);

  // invert bits
  memset(b, 0x5b, 50);

  b = lynx_realloc(b, 150);
  // first 50 bits should be inverted pattern; next 50 should be scribble
  // pattern.
  verify_contents(b, 0x5b, 50);
  verify_contents(b + 50, 0xb5, 100);

  void *c = lynx_calloc(50, 4);
  // calloc should still be 0
  verify_empty(c, 200);

  // validate that this also works with large blocks
  void *d = lynx_malloc(4 * MAX_BLOCK_ALLOC);
  verify_contents(d, 0xb5, 4 * MAX_BLOCK_ALLOC);

  lynx_free(a);
  lynx_free(b);
  lynx_free(c);
  lynx_free(d);

  print_lynx_alloc_debug_info();

  return 0;
}

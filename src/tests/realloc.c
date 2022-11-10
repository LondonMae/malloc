#include <stdlib.h>
#include <string.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates the behavior of realloc by growing and then shrinking an
 * array.
 */

int main(int argc, char **argv) {
  void *arr = NULL;
  size_t sizes[10] = {1000, 50, 100, 200, 0, 100, 200, 50, 300, 1000};
  char fill[10] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xab, 0xba, 0xde, 0xed};

  // on the first allocation, we should go from NULL to sizes[0] elements.
  arr = lynx_realloc(arr, sizes[0]);
  memset(arr, fill[0], sizes[0]);

  for (int i = 1; i < 10; i++) {
    arr = lynx_realloc(arr, sizes[i]);
    // verify that the previous values were carried over
    if (sizes[i] > 0) {
      verify_contents(arr, fill[i - 1], MIN(sizes[i - 1], sizes[i]));
      // fill with new values
      memset(arr, fill[i], sizes[i]);
    }
  }

  lynx_free(arr);
  print_lynx_alloc_debug_info();

  return 0;
}

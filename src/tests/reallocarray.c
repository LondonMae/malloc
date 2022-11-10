#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test validates the behavior of reallocarray by growing and then
 * shrinking an array. It additionally tests for overflow.
 */

int main(int argc, char **argv) {
  void *arr = NULL;
  size_t sizes[10] = {1000, 50, 100, 200, 0, 100, 200, 50, 300, 1000};
  char fill[10] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0xab, 0xba, 0xde, 0xed};

  // on the first allocation, we should go from NULL to sizes[0] elements.
  arr = lynx_reallocarray(arr, sizes[0], 1);
  memset(arr, fill[0], sizes[0]);

  size_t a = 0;
  a = ~a & (a - 1); // a should be 011...111
  // verify that a * 4 overflows
  size_t c = a * 4;
  EXPECT_NEQ(4, c / a, "overflow was expected...");

  for (int i = 1; i < 10; i++) {
    // try to overflow
    void *discard = lynx_reallocarray(arr, a, 4);
    EXPECT_EQ(NULL, discard, "expected NULL for realloc with overflow");
    EXPECT_EQ(ENOMEM, errno, "expected ENOMEM");

    // validate that we can resize without overflow and that the original
    // pointer is unaffected
    arr = lynx_reallocarray(arr, sizes[i], 1);
    if (sizes[i] > 0) {
      // verify that the previous values were carried over
      verify_contents(arr, fill[i - 1], sizes[i - 1]);
      // fill with new values
      memset(arr, fill[i], sizes[i]);
    }
  }

  lynx_free(arr);
  print_lynx_alloc_debug_info();

  return 0;
}

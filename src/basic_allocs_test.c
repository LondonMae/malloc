#include <stdio.h>

#include "lynx_alloc.h"

int main(int argc, char **argv) {
  print_lynx_alloc_debug_info();

  void *ptrs[10];
  // do a couple of allocs of different sizes
  printf("allocating 10...\n");
  for (int i = 0; i < 10; i++) {
    size_t size = 1 << i;
    ptrs[i] = lynx_malloc(size);
    printf("%p - %zu\n", ptrs[i], size);
  }

  print_lynx_alloc_debug_info();

  /*
  // free every other
  printf("freeing 5...\n");
  for (int i = 0; i < 10; i += 2) {
    printf("freeing %p\n", ptrs[i]);
    lynx_free(ptrs[i]);
  }

  print_lynx_alloc_debug_info();

  // alloc every other
  printf("allocating 5...\n");
  for (int i = 8; i >= 0; i -= 2) {
    size_t size = 1 << i;
    ptrs[i] = lynx_malloc(size);
    printf("%p - %zu\n", ptrs[i], size);
  }

  print_lynx_alloc_debug_info();
  */

  // free every pair (left)
  for (int i = 0; i < 9; i += 3) {
    printf("freeing %d and %d\n", i, i + 1);
    lynx_free(ptrs[i]);
    lynx_free(ptrs[i + 1]);
  }

  print_lynx_alloc_debug_info();

  // realloc every pair
  for (int i = 0; i < 9; i += 3) {
    printf("realloc %d and %d\n", i, i + 1);
    ptrs[i] = lynx_malloc(1 << i);
    ptrs[i + 1] = lynx_malloc(1 << (i + 1));
  }

  print_lynx_alloc_debug_info();

  // free them all
  printf("freeing everything...\n");
  for (int i = 0; i < 10; i++) {
    printf("freeing %p\n", ptrs[i]);
    lynx_free(ptrs[i]);
  }

  print_lynx_alloc_debug_info();
}

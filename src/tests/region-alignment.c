#include <stdlib.h>
#include <sys/mman.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * Test that we can align a region even if we receive an unaligned memory
 * mapping.
 */

#define PAGE_SIZE 4096

int main(int argc, char **argv) {
  setenv(REGION_SIZE_ENV_VAR, "32768", 1); // regions are 32k
  // before we do anything, map three pages; this will almost guarantee we are
  // misaligned.
  void *pages = mmap(NULL, PAGE_SIZE * 3, PROT_READ | PROT_WRITE,
                     MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  EXPECT_NEQ(MAP_FAILED, pages, "could not map memory");

  void *ptrs[20]; // allocate 10k of memory
  for (int i = 0; i < 20; i++) {
    ptrs[i] = lynx_malloc(513);
  }

  for (int i = 0; i < 20; i++) {
    lynx_free(ptrs[i]);
  }

  munmap(pages, PAGE_SIZE * 3);

  print_lynx_alloc_debug_info();

  return 0;
}

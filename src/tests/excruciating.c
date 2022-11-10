#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

/*
 * This test stress tests the allocator. It allocates and frees a large number
 * of pointers, writing over each.
 *
 * There are no expectations other than the allocator can accomodate all of the
 * requests and that the memory is cleaned up by free.
 */

// Default parameters:
// - 1M iterations
// - 1024 pointers
// - 5% of allocations are large
// - 10% of frees are reallocs
// - block sizes are [1, 10MB)
#define N_ITERS (1 << 20)
#define N_PTRS 1024
#define MIN_SIZE 1
#define MAX_SIZE 2049
#define LARGE_PCT 0.05
#define REALLOC_PCT 0.1
#define LARGE_MAX_SIZE (10 * (1 << 20))

// flip a coin
double c() { return ((double)rand()) / RAND_MAX; }

// rand in range
int r(int min, int max) {
  double coin = c();
  return min + (int)(coin * (max - min));
}

// pick a size
int s() {
  if (c() < LARGE_PCT) {
    return r(MAX_SIZE + 1, LARGE_MAX_SIZE);
  }
  return r(MIN_SIZE, MAX_SIZE);
}

void scribble(void *ptr, size_t size) { memset(ptr, c() * 256, size); }

char *rs(size_t s) {
  static char buff[80];
  static char *unit[4] = {"b", "kb", "mb", "gb"};
  int i;
  for (i = 3; i > 0; i--) {
    if (s > (1 << (i * 10))) {
      break;
    }
  }
  sprintf(buff, "%'lu %s", s >> (i * 10), unit[i]);
  return buff;
}

int main(int argc, char **argv) {
  setlocale(LC_NUMERIC, "");
  init_memory_tracking();

  if (argc > 1 && strncmp(argv[1], "-d", 2) == 0) {
    srand(42); // use a deterministic seed
  } else {
    srand(time(NULL)); // seed random
  }

  void *ptrs[N_PTRS];
  size_t sizes[N_PTRS];
  memset(ptrs, 0, sizeof(void *) * N_PTRS);  // NULL
  memset(sizes, 0, sizeof(size_t) * N_PTRS); // NULL

  size_t usage = 0;
  size_t peak_usage = 0;

  // use stderr to have unbuffered output
  fprintf(stderr, "starting 🫠\n");
  for (int i = 0; i < N_ITERS; i++) {
    // pick a pointer at random.
    int idx = r(0, N_PTRS);
    void *ptr = ptrs[idx]; // check out a pointer
    size_t size = sizes[idx];
    size_t nsize = s();
    if (ptr) {
      // ptr is not NULL; free or realloc
      if (c() < REALLOC_PCT) {
        ptr = lynx_realloc(ptr, nsize);
        scribble(ptr, nsize);
        usage -= size;
        usage += nsize;
      } else {
        lynx_free(ptr);
        usage -= size;
        nsize = 0;
        ptr = NULL;
      }
    } else {
      // ptr is NULL; alloc
      EXPECT_EQ(0, size, "invalid accounting");
      ptr = lynx_malloc(nsize);
      scribble(ptr, nsize);
      usage += nsize;
    }
    ptrs[idx] = ptr; // return our pointer
    sizes[idx] = nsize;
    peak_usage = MAX(usage, peak_usage);
    if (i % 1024 == 0) {
      fprintf(stderr, ".");
    }
  }
  fprintf(stderr, "\ndone! 🫠\n");

  // whew.
  for (int i = 0; i < N_PTRS; i++) {
    lynx_free(ptrs[i]);
  }

  checkpoint_memory();
  struct tracked_memory t = tracked_memory();

  // we should be cleaned up!
  printf("allocations: %'d\n", N_ITERS);
  printf("peak usage: %s\n", rs(peak_usage));
  printf("regions used at end: %lu\n", t.regions[1]);
  printf("large blocks used at end: %lu\n", t.large_blocks[1]);
  EXPECT_EQ(0, t.large_blocks[1], "large blocks should all be freed");
  EXPECT_EQ(0, t.regions[1], "regions should be gc'd");
  print_lynx_alloc_debug_info(); // lets see the counters!
  return 0;
}

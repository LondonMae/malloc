#include "test_utils.h"

#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../lynx_alloc.h"

uint64_t total_mem_alloc() {
  static char buff[PATH_MAX];
  pid_t pid = getpid();
  sprintf(buff, "/proc/%d/statm", pid);
  FILE *f = fopen(buff, "r");
  uint64_t usage;
  fscanf(f, "%lu ", &usage);
  fclose(f);
  return usage;
}

void verify_contents(void *arr, char expected, size_t size) {
  char *a = (char *)arr;
  for (int i = 0; i < size; i++) {
    EXPECT_EQ(expected, a[i], "memory contents to not match expected");
  }
}

void verify_empty(void *arr, size_t size) { verify_contents(arr, 0x00, size); }

#define MAX_TRACKED_EVENTS 1024

// TODO just keep an array of structs, this is ugly
uint64_t pages[MAX_TRACKED_EVENTS];
uint64_t regions[MAX_TRACKED_EVENTS];
uint64_t large_blocks[MAX_TRACKED_EVENTS];
int tracking_idx;

void init_memory_tracking() {
  lynx_alloc_init();
  memset(pages, 0, MAX_TRACKED_EVENTS * sizeof(uint64_t));
  memset(regions, 0, MAX_TRACKED_EVENTS * sizeof(uint64_t));
  memset(large_blocks, 0, MAX_TRACKED_EVENTS * sizeof(uint64_t));
  tracking_idx = 0;
  checkpoint_memory();
}
void checkpoint_memory() {
  struct malloc_counters counters = lynx_alloc_counters();
  pages[tracking_idx] = total_mem_alloc();
  regions[tracking_idx] = counters.region_allocs - counters.region_frees;
  large_blocks[tracking_idx] =
      counters.large_block_allocs - counters.large_block_frees;
  tracking_idx++;
}

struct tracked_memory tracked_memory() {
  struct tracked_memory t = {pages, regions, large_blocks};
  return t;
}

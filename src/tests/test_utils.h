#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#include <stdint.h>
#include <stdio.h>

// verify contents of a buffer
void verify_contents(void *arr, char expected, size_t size);
void verify_empty(void *arr, size_t size);

// returns the number of pages allocated to the process.
uint64_t total_mem_alloc();

#define MAX_TRACKED_EVENTS 1024

// initializes tracking of memory; this will checkpoint the number of used pages
// and allocator regions immediately. subsequent calls to checkpoint_memory()
// will take a snapshot of the used pages/regions at the point of the call.
void init_memory_tracking();
void checkpoint_memory();

struct tracked_memory {
  // null-terminated arrays of tracked pages and regions.
  uint64_t *pages;
  uint64_t *regions;
  uint64_t *large_blocks;
};

struct tracked_memory tracked_memory();

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// Test expectation macros

#define EXPECT(expr, message)                                                  \
  do {                                                                         \
    if (!(expr)) {                                                             \
      printf("Assertion %s failed: %s (%s:%d:%s)\n", "" #expr "", message,     \
             __FILE__, __LINE__, __func__);                                    \
      exit(1);                                                                 \
    }                                                                          \
  } while (0);

#define EXPECT_TRUE(expr, message) EXEPCT(expr, message)
#define EXPECT_FALSE(expr, message) EXPECT(!(expr), message)

#define EXPECT_OPERATOR(val0, val1, OP, message) EXPECT(val0 OP val1, message)

#define EXPECT_LT(val0, val1, message) EXPECT_OPERATOR(val0, val1, <, message)
#define EXPECT_LTE(val0, val1, message) EXPECT_OPERATOR(val0, val1, <=, message)
#define EXPECT_GT(val0, val1, message) EXPECT_OPERATOR(val0, val1, >, message)
#define EXPECT_GTE(val0, val1, message) EXPECT_OPERATOR(val0, val1, >=, message)
#define EXPECT_EQ(val0, val1, message) EXPECT_OPERATOR(val0, val1, ==, message)
#define EXPECT_NEQ(val0, val1, message) EXPECT_OPERATOR(val0, val1, !=, message)

#endif

#ifndef __LYNX_ALLOC_H__
#define __LYNX_ALLOC_H__

#include <stdint.h>
#include <stdlib.h>

// ------ Malloc tunable parameters and environment variables ------
// Default region size -- equal to one page. See documentation of regions,
// below.
#define DEFAULT_REGION_SIZE 4096
#define REGION_SIZE_ENV_VAR "MALLOC_REGION_SIZE"
// Max block size -- blocks larger than this will be allocated as independent
// blocks using mmap; blocks smaller than this will be packed within a region.
#define MAX_BLOCK_ALLOC 2048
#define MAX_BLOCK_ALLOC_ENV_VAR "MALLOC_MAX_BLOCK"
// Capacity to reserve at the end of a block. This is useful when workloads use
// of realloc and it would be useful to have free space at the end of the block.
#define RESERVE_CAPACITY 0
#define RESERVE_CAPACITY_ENV_VAR "MALLOC_RESERVE_CAPACITY"
// Minimum remainder size to split a block. If a free location has less padding
// than this value, the block will not be split.
#define MIN_SPLIT_SIZE 128
#define MIN_SPLIT_ENV_VAR "MALLOC_MIN_SPLIT"
// Set this environment variable to a one-byte char (in hex representation) to
// set a value to "scribble" memory with. All allocated blocks will be scribbled
// (i.e., filled) with this character. This can be useful for detecting memory
// errors (e.g., if code assumes that all newly-allocated values are zeroed).
#define DEFAULT_SCRIBBLE_CHAR 0x00
#define SCRIBBLE_ENV_VAR "MALLOC_SCRIBBLE"

// A block of managed memory. Blocks have a header and a footer section
// describing the size. A the start address of the block immediately follows it
// and is always a multiple of 16. The end of a block contains the metadata for
// the next block. i.e.,
//
//       | block0 data |           | block1 data ...
// | hdr | data ...    | ftr | hdr | data ...
//       |<---block0 size (%16)--->|<---block1 size ...
//       ^
//       16-byte aligned
//
// Block sizes are a multiple of 16.
//
// Large blocks are blocks that are not arranged in block lists. Any request to
// allocate a block that is larger than the config.max_block_size (see config
// struct below) will be allocated directly with mmap. The default for this
// value is 2048. Large blocks still require metadata (the size of the mmap'ed
// region), so they still have a header.
//
// A large block has a slightly different format:
//
// | padding | hdr | block data ...  |
//  < -------------^- block size --->
//                 |
//                 16-byte aligned
//
// For a large block, the block size is the size of the _entire mmap'ed region_.
// This is in contrast to a 'normal' block, where the size represents the size
// of the block _excluding the header_.
//
// The data in a large block is still 16-byte aligned.
//
// Block size types are 4-byte values, so the padding in a large block is 12
// bytes.
typedef uint32_t block_t;

// A region is a large allocation of managed memory that contains blocks.
//
// All regions start with some metadata about the region, including pointers to
// the previous/next region and the number of used and free blocks within the
// region.
//
// The region metadata also contains a pointer to the header of the first block
// in the region. Other blocks are found by traversing the (implicit) block list
// for the region.
//
// The list of blocks starts after this region metadata. The block list starts
// with a small initial block that is marked as used (to prevent attempting to
// coalesce it). The block list ends with a block of size zero that is marked as
// used.
//
// The allocator maintains a linked list of regions.
//
// See the documentation of the region_create() function in lynx_alloc.c for
// more information about these initial/final blocks.
typedef struct region region_t;
struct region {
  block_t *block_list; // free list for region; should be directly after
                       // null_header
  region_t *next;      // next region
  region_t *prev;      // prev region; optimization for cleaning up free regions
  uint32_t n_free;     // number of free blocks in the region
  uint32_t n_used;     // number of used blocks in the region (ignoring
                       // intro/outro)
};

// Tuning parameters.
//
// These parameters control the size of regions and large blocks for the
// allocator, as well as how small the minimum block split should be.
//
// See documentation of environment variables/constants above for more
// information.
struct malloc_config {
  size_t region_size;
  size_t max_block_size;
  size_t reserve_capacity; // TODO: unused
  size_t min_split_size;
  char scribble_char;
};

// Counters used for debugging.
struct malloc_counters {
  // region counters
  uint64_t region_allocs;
  uint64_t region_frees;
  // block counters
  uint64_t total_allocs;
  uint64_t total_frees;
  // large block counters
  uint64_t large_block_allocs;
  uint64_t large_block_frees;
};

// The  malloc()  function  allocates size bytes and returns a pointer to the
// allocated memory.  The memory is not initialized.  If size is 0, then
// malloc() returns either NULL, or a unique  pointer value that can later be
// successfully passed to free.
void *lynx_malloc(size_t size);

// The free() function frees the memory space pointed to by ptr, which must
// have been returned by a previous call to malloc(), calloc(), or realloc().
// Otherwise, or if free(ptr) has already been called before, undefined
// behavior occurs. If ptr is NULL, no operation is performed.
void lynx_free(void *ptr);

// The calloc() function allocates memory for an array of nmemb elements of
// size bytes each and returns a pointer to the allocated memory. The memory is
// set to zero. If nmemb or size is 0, then calloc() returns either
// NULL, or a unique pointer value that can later be successfully passed to
// free().
void *lynx_calloc(size_t nmemb, size_t size);

// The realloc() function changes the size of the memory block pointed to by ptr
// to size bytes. The contents will be unchanged in the range from the start of
// the region up to the minimum of the old and new sizes. If the new size is
// larger than the old size, the added memory will not be initialized.  If
// ptr is NULL, then the call is equivalent to malloc(size), for all values of
// size; if size is equal to zero, and ptr is not NULL, then the call is
// equivalent to free(ptr).  Unless ptr is NULL, it must have been returned
// by an earlier call to malloc(), calloc(), or realloc(). If the area pointed
// to was moved, a free(ptr) is done.
void *lynx_realloc(void *ptr, size_t size);

// The reallocarray() function changes the size of the memory block pointed to
// by ptr to be large enough for an array of nmemb elements, each of which is
// size bytes. It is equivalent to the call
//
//        realloc(ptr, nmemb * size);
//
// However, unlike that realloc() call, reallocarray() fails safely in the case
// where the multiplication would overflow. If such an overflow occurs,
// reallocarray() returns NULL, sets errno to ENOMEM, and leaves the original
// block of memory unchanged.
void *lynx_reallocarray(void *ptr, size_t nmemb, size_t size);

// print debug info
void print_lynx_alloc_debug_info();

// get config
struct malloc_config lynx_alloc_config();

// get counters
struct malloc_counters lynx_alloc_counters();

// perform initialization
void lynx_alloc_init();

#endif

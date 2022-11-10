#include "lynx_alloc_shared.h"

#include <errno.h>
#include <stdlib.h>

#include "lynx_alloc.h"

void *malloc(size_t size) { return lynx_malloc(size); }

void free(void *ptr) { lynx_free(ptr); }

void *realloc(void *ptr, size_t size) { return lynx_realloc(ptr, size); }

void *calloc(size_t nmemb, size_t size) { return lynx_calloc(nmemb, size); }

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  return lynx_reallocarray(ptr, nmemb, size);
}

// This prevents your malloc from being used in multi-threaded
// applications.
int pthread_create(void __attribute__((unused)) * x, ...) { exit(-ENOSYS); }

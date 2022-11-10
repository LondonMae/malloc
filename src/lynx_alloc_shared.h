#ifndef __LYNX_ALLOC_SHARED_H__
#define __LYNX_ALLOC_SHARED_H__

#include "lynx_alloc.h"

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);

#endif

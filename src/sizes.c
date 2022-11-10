#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  printf("sizeof(unsigned int)\t= %zu\n", sizeof(unsigned int));
  printf("sizeof(void*)\t\t= %zu\n", sizeof(void *));
  printf("sizeof(unsigned long)\t= %zu\n", sizeof(unsigned long));
  printf("sizeof(uint32_t)\t= %zu\n", sizeof(uint32_t));
  printf("sizeof(uint64_t)\t= %zu\n", sizeof(uint64_t));
  printf("sizeof(uintptr_t)\t= %zu\n", sizeof(uintptr_t));
}

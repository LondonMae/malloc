#include <stdlib.h>

#include "../lynx_alloc.h"
#include "test_utils.h"

int main(int argc, char **argv) {
  init_memory_tracking();

  checkpoint_memory();
  struct tracked_memory t = tracked_memory();
  return 0;
}

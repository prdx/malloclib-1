#ifndef MALLOC_ALIGNMENT
#define MALLOC_ALIGNMENT 8
#endif /* MALLOC_ALIGNMENT */

#include "mallutl.h"


int myposix_memalign(void** pp, size_t alignment, size_t bytes) {
  void *mem = 0;
  if(mem == 0) {
    return ENOMEM;
  }
}

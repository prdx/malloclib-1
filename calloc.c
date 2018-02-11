#include "mallutl.h"
#include "malloc.h"
#include <string.h>

/*------------CALLOC----------------*/
void *calloc(size_t num, size_t size) {
  void *addr = malloc(num * size);
  memset(addr, '\0', num * size);
  return addr;
}

#include "mallutl.h"
#include "realloc.h"
#include <string.h>

// Our custom malloc
#include "free.h"
#include "malloc.h"

size_t min(size_t, size_t);

/*-----------REALLOC---------------*/
void *realloc(void *ptr, size_t size) {
  if (ptr == NULL)
    return malloc(size);

  // Based on the documentation, if size = 0 but ptr is valid, free it instead
  if (size == 0 && ptr != NULL) {
    free(ptr);
    return NULL;
  }

  // Allocate new memory
  void *addr;
  if ((addr = malloc(size)) == NULL) {
    return NULL;
  }

  void *temp = (char *)ptr - sizeof(block_header_t);
  block_header_t *block = temp;

  // Always use the smaller size when copying
  // When the requested size is larger, copy whole data in old segment
  // If the new size is smaler, copy the data that can fit only
  size_t new_size = min(size, block->size - sizeof(block_header_t));

  memcpy(addr, ptr, new_size);
  free(ptr);

  return addr;
}

size_t min(size_t a, size_t b) { return a < b ? a : b; }

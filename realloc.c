#include "mallutl.h"
#include "malloc.h"
#include "math.h"
#include "realloc.h"
#include <string.h>
#include "free.h"
/*-----------REALLOC---------------*/
void *realloc(void *ptr, size_t size) {
  // Case handler when ptr is null, return new allocated memory
  if(ptr == NULL) return malloc(size);
  
  void *addr;
  if((addr = malloc(size)) == NULL) {
    return NULL;
  }
  header_t *header = get_header(ptr);
  if(header == NULL) return NULL;

  memcpy(addr, ptr, pow(2, header->size));
  free(ptr);

  return addr;
}

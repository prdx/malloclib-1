#include "mallutl.h"
#include "malloc.h"
#include "math.h"
#include <unistd.h>  /* for sbrk, sysconf */
#include "realloc.h"
#include <string.h>
#include "free.h"
/*-----------REALLOC---------------*/
void *realloc(void *ptr, size_t size) {
  /*char buf[1024];*/
  /*snprintf(buf, 1024, "%s:%d (Realloc) Requested: %zu of %p\n",*/
      /*__FILE__, __LINE__, size, ptr);*/
  /*write(STDOUT_FILENO, buf, strlen(buf) + 1);*/
  // Case handler when ptr is null, return new allocated memory
  if(ptr == NULL) return malloc(size);
  if(size == 0 && ptr != NULL) {
    free(ptr);
    return NULL;
  } 
  size_t copy_size;

  
  void *addr;
  if((addr = malloc(size)) == NULL) {
    return NULL;
  }
  header_t *header = get_header(ptr);
  if(size < pow(2, header->size)) {
    copy_size = size;
  }
  else {
    copy_size = pow(2, header->size);
  }
  if(header == NULL) return NULL;

  memcpy(addr, ptr, copy_size);
  free(ptr);

  return addr;
}

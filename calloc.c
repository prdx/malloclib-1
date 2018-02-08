#include "mallutl.h"
#include "malloc.h"
#include <stdio.h>
#include <unistd.h>  /* for sbrk, sysconf */
#include <string.h>
/*------------CALLOC----------------*/
void *calloc(size_t num, size_t size) {
  /*char buf[1024];*/
  /*snprintf(buf, 1024, "%s:%d (Calloc) Requested: %zu\n",*/
      /*__FILE__, __LINE__, size);*/
  /*write(STDOUT_FILENO, buf, strlen(buf) + 1);*/
  void *addr = malloc(num * size);
  /*memset(addr, '\0', size);*/
  return addr;
}

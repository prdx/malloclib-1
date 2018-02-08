#include "mallutl.h"
#include "realloc.h"
#include <unistd.h>  /* for sbrk, sysconf */
#include "reallocarray.h"
/*------------REALLOCARRAY---------------*/
/* 
 Equivalent to realloc(ptr, nmemb * size), except it handles int overflow
 Source: 
 https://www.gnu.org/software/libc/manual/html_node/Changing-Block-Size.html
*/
void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  char buf[1024];
  snprintf(buf, 1024, "%s:%d (Reallocarray) Requested: %zu\n",
      __FILE__, __LINE__,size);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
  size_t total = nmemb * size;
  // handle the integer overflow
  if(nmemb != 0 && total / nmemb != size) {
    errno = ENOMEM;
    return NULL;
  }

  void *addr = realloc(ptr, total);
  return addr;
}

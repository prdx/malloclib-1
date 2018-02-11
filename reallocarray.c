#include "mallutl.h"
#include "realloc.h"
#include "reallocarray.h"

/*------------REALLOCARRAY---------------*/
/*
 Equivalent to realloc(ptr, nmemb * size), except it handles int overflow
 Source:
 https://www.gnu.org/software/libc/manual/html_node/Changing-Block-Size.html
*/
void *reallocarray(void *ptr, size_t nmemb, size_t size) {
  size_t total = nmemb * size;
  // handle the integer overflow
  if (nmemb != 0 && total / nmemb != size) {
    errno = ENOMEM;
    return NULL;
  }

  void *addr = realloc(ptr, total);
  return addr;
}

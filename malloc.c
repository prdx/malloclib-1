#include <unistd.h>     /* for sbrk, sysconf */
#include "mallutl.h"    /* for data structure */

/* The memory request must be always multiple of PAGE_SIZE */
#ifndef malloc_getpagesize
#define malloc_getpagesize   sysconf(_SC_PAGE_SIZE)
#endif

/*
 * This function will get the initial heap memory to allocate (or probably other
 * information for malloc to perform)
 * We also need to store the information of the segment.
 *
*/

Node *head = NULL;

/*
  Buddy allocation malloc:
*/
void *malloc(size_t size) {

  /* 1. Initialize: when the list is empty, we create a new head and then get 
     memory from the OS. We should probably also check what is the size. 
     Since, we can directly ask the OS the (size / PAGE_SIZE + 1) * PAGE_SIZE 
  */
  size_t size_allocated = (size / malloc_getpagesize + 1) * malloc_getpagesize;
  if(head == NULL) {
    head = sbrk(size_allocated);
    head->size = size_allocated;
    head->next = NULL;
  }
}


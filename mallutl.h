#ifndef NO_MALLOC_STATS
#define NO_MALLOC_STATS 0
#endif /* NO_MALLOC_STATS */

/* 
  I learn that we have to reduce dependecy as much as 
  possible, so we will include stdio.h only when we need it.
*/
#if !NO_MALLOC_STATS
#include <stdio.h>      /* for printing and debugging */
#endif

#include <errno.h>      /* for MALLOC_FAILURE_ACTION */ 
#include <stddef.h>   /* for size_t */

/* By default errno is ENOMEM */
#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION   errno = ENOMEM;
#endif /* MALLOC_FAILURE_ACTION */

typedef struct __node_t {
  int             size;
  struct __node_t *next;
} node_t;
typedef node_t Node;

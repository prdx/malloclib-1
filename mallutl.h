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
#include <string.h>

/* By default errno is ENOMEM */
#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION   errno = ENOMEM;
#endif /* MALLOC_FAILURE_ACTION */

#include <pthread.h>
#define INIT_PTHREAD_MUTEX(mutex) ( memset(mutex, 0, sizeof(pthread_mutex_t) ))

#define MIN_ORDER 5
#define MAX_ORDER 12

typedef struct __header_t {
  void* address;
  size_t size;
  unsigned is_free;
  struct __header_t *next;
} header_t;

typedef struct __arena_t {
  pthread_mutex_t arena_lock;
  header_t* base_header;
  unsigned allocated;
  unsigned mmaped;
  size_t size;
  int header_index;
  struct __arena_t *next;
} arena_t;

extern arena_t *arenas;
extern arena_t *current_arena;
extern int arena_index;

extern pthread_mutex_t global_mutex;


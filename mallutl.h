#include <errno.h>      /* for MALLOC_FAILURE_ACTION */ 
#include <pthread.h>
#include <stdio.h>      /* for printing and debugging */
#include <stddef.h>   /* for size_t */
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>  /* for sbrk, sysconf */

/* By default errno is ENOMEM */
#ifndef MALLOC_FAILURE_ACTION
#define MALLOC_FAILURE_ACTION   errno = ENOMEM;
#endif /* MALLOC_FAILURE_ACTION */

#define MIN_ORDER 5
#define MAX_ORDER 12

/* The memory request must be always multiple of PAGE_SIZE */
#define HEAP_PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define BASE 2
#define SIZE_TO_ORDER(size) (ceil((log(size) / log(BASE))))

typedef struct __block_header_t {
  void* address;
  unsigned order;
  size_t size;
  unsigned is_free;
  unsigned position;
  unsigned is_mmaped;
  struct __block_header_t *next;
} block_header_t;

enum free_status {occupied = 0, empty = 1};
enum position {left = 0, right = 1};
enum is_mmaped {allocated = 0, mmaped = 1};

extern block_header_t *head;
extern block_header_t *tail;
extern pthread_mutex_t global_mutex;


#include <unistd.h>     /* for sbrk, sysconf */
#include "mallutl.h"    /* for data structure */
#include <math.h>

/* The memory request must be always multiple of PAGE_SIZE */
#define HEAP_PAGE_SIZE   sysconf(_SC_PAGE_SIZE)
#define BASE 2
#define DATA_OFFSET     offsetof(header_t, block_data)
#define SIZE_TO_ORDER(size) ( ceil( (log(size)/log(BASE)) ) )
#define MAX_ORDER 12 /* 2^12 = 4096 */

void *find_suitable_space(size_t);
int init(size_t);

/*
 * This function will get the initial heap memory to allocate (or probably other
 * information for malloc to perform)
 * We also need to store the information of the segment.
 *
*/

arena_t* arenas = NULL;
int arena_index = 0;

/*
  Buddy allocation malloc:
*/
void *malloc(size_t block) {
  void *addr = NULL;

  // if no empty block found, create new arena
  if(init(block) == -1) {
    MALLOC_FAILURE_ACTION;
    return NULL;
  }
}

int init(size_t block) {
  void *addr;
  int count = 0;

  unsigned data_size = block / HEAP_PAGE_SIZE + 1;
  size_t allocated_memory = sizeof(arena_t) +  /* Arena header */
    sizeof(header_t) * data_size * 512 +       /* Block headers */
    data_size * HEAP_PAGE_SIZE;                /* Data */

  if((addr = sbrk(allocated_memory)) == (void*) -1) {
    return -1;
  }

  if(arenas == NULL) {
    arenas = (arena_t*) addr;
    header_t* header = (header_t*)(addr + sizeof(arena_t));
    void* data = addr + sizeof(header_t) * data_size * 512;
    arenas->base_header = header;
    arenas->next = NULL;
    arenas->size = allocated_memory;
    header->address = data;
    header->next = NULL;
    header->is_free = 1;
    header->size = SIZE_TO_ORDER(block);
    count = 1;
  }
  else {
    count = arena_index;
    arena_t *arena = arenas;
    while(arena->next != NULL) {
      arena = arena->next;  /* get the tail */
    }
    arena->next = addr;     /* add new node */ 
    arena = arena->next;    /* move to the new node */
    header_t* header = (header_t*)(addr + sizeof(arena_t));
    void* data = addr + sizeof(header_t) * data_size * 512;
    arena->base_header =  header;
    arena->next = NULL;
    arena->size = allocated_memory;
    header->address = data;
    header->next = NULL;
    header->is_free = 1;
    header->size = SIZE_TO_ORDER(block);
    count++;
  }
  return count;
}

void* find_suitable_space(size_t block) {
  arena_t *arena = arenas;
  while(arena->next != NULL) {
    arena = arena->next;
  }
}


/* 
 Author: Anak Agung Ngurah Bagus Trihatmaja
 Malloc library using buddy allocation algorithm
*/

#include <unistd.h>  /* for sbrk, sysconf */
#include "mallutl.h" /* for data structure */
#include "malloc.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

/* The memory request must be always multiple of PAGE_SIZE */
#define HEAP_PAGE_SIZE sysconf(_SC_PAGE_SIZE)
#define BASE 2
#define DATA_OFFSET offsetof(header_t, block_data)
#define SIZE_TO_ORDER(size) (ceil((log(size) / log(BASE))))
#define MAX_ORDER 12 /* 2^12 = 4096 */

void *find_suitable_space(size_t);
int init(size_t);
int is_need_split(header_t *, size_t);
void split(header_t *, size_t);
void add_new_header(header_t *);

int arena_index = 0;
arena_t *arenas = NULL;
arena_t *current_arena = NULL;
pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;


/*------------MALLOC---------------*/
void *malloc(size_t block) {
  char buf[1024];
  snprintf(buf, 1024, "%s:%d Requested: %zu\n",
      __FILE__, __LINE__, block);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
  header_t *addr = NULL;

  if(block < pow(2, MIN_ORDER)) 
          block = pow(2, MIN_ORDER);

  // if no empty block found, create new arena
  pthread_mutex_lock(&global_mutex);

  // Skip finding space if request is large
  if(block > 4096) {
    if ((arena_index = init(block)) == -1) {
      MALLOC_FAILURE_ACTION;
      return NULL;
    }
    addr = current_arena->base_header;
  }
  else {
    if ((addr = find_suitable_space(block)) == (void *)-1) {
      if ((arena_index = init(block)) == -1) {
        MALLOC_FAILURE_ACTION;
        return NULL;
      }
      addr = current_arena->base_header;
    }
    arena_t *test = arenas;
    while (test != NULL) {
      snprintf(buf, 1024, "%s:%d  %p\n",
          __FILE__, __LINE__, test);
      write(STDOUT_FILENO, buf, strlen(buf) + 1);
      test = test->next; /* get the tail */
    }
    // found empty block, either split or just fill it
    if (is_need_split(addr, block)) {
      split(addr, block);
    }
  }
  addr->is_free = 0;
  pthread_mutex_unlock(&global_mutex);
  return addr->address;
}

int init(size_t block) {
  void *addr;
  int count = 0;

  unsigned data_size = block / HEAP_PAGE_SIZE + 1;
  size_t allocated_memory;
  if(block <= 4096) {
    allocated_memory = 
      sizeof(arena_t) +                    /* Arena header */
      sizeof(header_t) *  512 +            /* Block headers */
      data_size * HEAP_PAGE_SIZE;          /* Data */

    if ((addr = sbrk(allocated_memory)) == (void *)-1) {
      return -1;
    }
  }
  else {
    allocated_memory = sizeof(arena_t) + sizeof(header_t) + data_size * HEAP_PAGE_SIZE;
    if ((addr = mmap(0, allocated_memory, PROT_READ | PROT_WRITE,
                   MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == (void *)-1) {
      return -1;
    }
  }

  void *data;

  if (arenas == NULL) {
    /* Assign value to the node */
    arenas = (arena_t *)addr;
    header_t *header = (header_t *)(addr + sizeof(arena_t));
    arenas->base_header = header;
    arenas->next = NULL;
    arenas->size = allocated_memory;
    arenas->header_index = 1;
    if(data_size == 1) {
      arenas->allocated = 1;
      data = addr + sizeof(header_t) * data_size * 512;
    }
    else {
      arenas->mmaped = 1;
      data = addr + sizeof(header_t);
    }
    current_arena = arenas;
    header->address = data;
    header->next = NULL;
    header->is_free = 1;
    header->size = SIZE_TO_ORDER(data_size * HEAP_PAGE_SIZE);
    count = 1;
  } else {
    count = arena_index;
    arena_t *arena = arenas;

    while (arena->next != NULL) {
      arena = arena->next; /* get the tail */
    }

    arena->next = addr;  /* add new node */
    arena = arena->next;
    /* assign value to the node */
    header_t *header = (header_t *)(addr + sizeof(arena_t));
    arena->base_header = header;
    if(data_size == 1) {
      arena->allocated = 1;
      data = addr + sizeof(header_t) * data_size * 512;
    }
    else {
      arena->mmaped = 1;
      data = addr + sizeof(header_t);
    }
    arena->next = NULL;
    arena->size = allocated_memory;
    arena->header_index = 1;
    header->address = data;
    header->next = NULL;
    header->is_free = 1;
    header->size = SIZE_TO_ORDER(data_size * HEAP_PAGE_SIZE);
    current_arena = arena;
    count++;
  }
  return count;
}

void *find_suitable_space(size_t block) {
  if (arenas == NULL)
    return (void *)-1;
  arena_t *arena = arenas;

  while (arena != NULL) {
    
    char buf[1024];
    snprintf(buf, 1024, "%s:%d  %p\n",
        __FILE__, __LINE__, arena);
    write(STDOUT_FILENO, buf, strlen(buf) + 1);
    header_t *header = arena->base_header;
    while (header != NULL) {
      if(header->is_free == 1) {
        if (pow(2, header->size) >= block) {
          current_arena = arena;
          return header;
        }
      }
      header = header->next;
    }
    arena = arena->next; /* get the tail */
  }
  return (void *)-1;
}

int is_need_split(header_t *header, size_t block) {
  if (header->is_free == 1 && pow(2, header->size) / 2 < block) {
    return 0;
  }
  return 1;
}

void split(header_t *header, size_t block) {
  if ((pow(2, header->size) / 2) <= block && pow(2, header->size) > block) {
    return;
  }

  // Adding new node, is to put the the new node in the
  // head address + sizeof(header_t)
  add_new_header(header);
  split(header, block);
}

void add_new_header(header_t *header) {
  header_t *current = header;
  header_t *new =
      current_arena->base_header + sizeof(header) * current_arena->header_index;
  new->is_free = 1;
  new->size = header->size - 1;
  new->next = NULL;
  int offset = pow(2, new->size);
  new->address = header->address + offset;

  // reduce the size of current header as well
  current->size = new->size;

  header_t *temp = current->next;
  new->next = temp;
  current->next = new;
  current_arena->header_index += 1;
}

/* Printing all the nodes */
void debug_info() {
  arena_t *arena = arenas;
  header_t *header = NULL;
  while (arena != NULL) {
    header = arena->base_header;
    while (1) {
      printf("----------------NODE---------------\n");
      printf("Location: %p\n", header);
      printf("Data address: %p\n", header->address);
      printf("Order: %d\n", (int)header->size);
      printf("Free status: %d\n", header->is_free);
      printf("Next: %p\n", header->next);
      printf("------------------------------------\n");
      if (header->next == NULL) {
        break;
      }
      header = header->next;
    }
    arena = arena->next;
  }
}

/*------------MEMALIGN----------------*/
/*
 allocate size bytes whose address is multiple of boundart
 boundary is a power of two
*/
/*void *memalign(size_t boundary, size_t size) {*/
  /*// boundary must be power of two*/
  /*if(ceil(log2(boundary)) != floor(log2(boundary))) {*/
    /*errno = EINVAL;*/
    /*return NULL;*/
  /*}*/
  
  /*return malloc(size);*/
/*}*/

/*-----------POSIX_MEMALIGN-----------*/
/*int posix_memalign(void **memptr, size_t alignment, size_t size) {*/
  /*// alignment must be power of two*/
  /*if(ceil(log2(alignment)) != floor(log2(alignment))) {*/
    /*errno = EINVAL;*/
    /*return -1;*/
  /*}*/
  /*return 0;*/
  /*// Use mmap here, remember to add node so it can be freed*/
  /*// We might need to change a bit the design, to use mmap if size is greater*/
  /*// than some value*/
/*}*/

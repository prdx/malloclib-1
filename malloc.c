/* 
 Author: Anak Agung Ngurah Bagus Trihatmaja
 Malloc library using buddy allocation algorithm
 TODO: Implement handler when all blocks are full
*/

#include <unistd.h>  /* for sbrk, sysconf */
#include "mallutl.h" /* for data structure */
#include "malloc.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

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

void *align8(void *x)
{
    unsigned long p = (unsigned long)x;
    p = (((p - 1) >> 3) << 3) + 8;
    return (void *)p;
}

/*------------MALLOC---------------*/
void *malloc(size_t block) {
  header_t *addr = NULL;

  // if no empty block found, create new arena
  if ((addr = find_suitable_space(block)) == (void *)-1) {
    if ((arena_index = init(block)) == -1) {
      MALLOC_FAILURE_ACTION;
      return NULL;
    }
    addr = arenas->base_header;
  }

  // found empty block, either split or just fill it
  if (is_need_split(addr, block)) {
    split(addr, block);
  }

  addr->is_free = 0;
  char buf[1024];
  snprintf(buf, 1024, "%s:%d Debug, returned: %p\n",
           __FILE__, __LINE__, addr->address);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
  return align8(addr->address);
}


int init(size_t block) {
  void *addr;
  int count = 0;

  unsigned data_size = block / HEAP_PAGE_SIZE + 1;
  size_t allocated_memory =
      sizeof(arena_t) +                    /* Arena header */
      sizeof(header_t) * data_size * 512 + /* Block headers */
      data_size * HEAP_PAGE_SIZE;          /* Data */

  if ((addr = sbrk(allocated_memory)) == (void *)-1) {
    return -1;
  }

  if (arenas == NULL) {
    /* Assign value to the node */
    arenas = (arena_t *)addr;
    header_t *header = (header_t *)(addr + sizeof(arena_t));
    void *data = addr + sizeof(header_t) * data_size * 512;
    arenas->base_header = header;
    arenas->next = NULL;
    arenas->size = allocated_memory;
    arenas->header_index = 1;
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
    arena = arena->next; /* move to the new node */
    current_arena = arena;
    /* assign value to the node */
    header_t *header = (header_t *)(addr + sizeof(arena_t));
    void *data = addr + sizeof(header_t) * data_size * 512;
    arena->base_header = header;
    arena->next = NULL;
    arena->size = allocated_memory;
    arena->header_index = 1;
    header->address = data;
    header->next = NULL;
    header->is_free = 1;
    header->size = SIZE_TO_ORDER(data_size * HEAP_PAGE_SIZE);
    count++;
  }
  return count;
}

void *find_suitable_space(size_t block) {
  if (arenas == NULL)
    return (void *)-1;
  arena_t *arena = arenas;

  while (arena != NULL) {
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

/*------------FREE---------------*/

header_t *get_header(void*);
void merge_if_possible(header_t*);

void free(void* address) {
  // We can't use printf here because printf internally calls `malloc` and thus
  // we'll get into an infinite recursion leading to a segfault.
  // Instead, we first write the message into a string and then use the `write`
  // system call to display it on the console.

  header_t *header = get_header(address);
  if(address == NULL || header == NULL) {
    return;
  }
  
  header->is_free = 1;

  arena_t *arena = arenas;
  while (arena != NULL) {
    merge_if_possible(arena->base_header); 
    arena = arena->next;
  }
  char buf[1024];
  snprintf(buf, 1024, "%s:%d Debug, address: %p\n",
           __FILE__, __LINE__, address);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
}

header_t *get_header(void* address) {
  arena_t *arena = arenas;
  header_t *header = NULL;

  while(arena != NULL) {
    header = arena->base_header;
    while(header != NULL) {
      if(header->address == address) {
        return header;
      }
      header = header->next;
    }
    arena = arena->next;
  }

  return NULL;
}

void merge_if_possible(header_t *header) {
  if(header->next == NULL) {  /* last element */
    return;
  }
  header_t *current = header;

  while(current->next != NULL) {
    if(current->size == current->next->size) {
      if(current->is_free && current->next->is_free) {
        current->size += 1;
        header_t* temp = current->next;
        current->next = temp->next;
        temp = NULL;
        current = header;
        continue;
      }
      current = current->next->next;
    }
    else {
      current = current->next;
    }
  }
}

/*------------CALLOC----------------*/
void *calloc(size_t num, size_t size) {
  void *addr = malloc(num * size);
  memset(addr, '\0', size);
  return addr;
}

/*-----------REALLOC---------------*/
void *realloc(void *ptr, size_t size) {
  // Case handler when ptr is null, return new allocated memory
  if(ptr == NULL) return malloc(size);
  
  void *addr;
  if((addr = malloc(size)) == NULL) {
    return NULL;
  }
  header_t *header = get_header(ptr);
  if(header == NULL) return NULL;

  memcpy(addr, ptr, pow(2, header->size));
  free(ptr);

  return addr;
}

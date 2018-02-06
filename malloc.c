/* 
 Author: Anak Agung Ngurah Bagus Trihatmaja
 Malloc library using buddy allocation algorithm
 TODO: Implement handler when all blocks are full
*/

#include <unistd.h>  /* for sbrk, sysconf */
#include "mallutl.h" /* for data structure */
#include <math.h>

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

/*
  Buddy allocation malloc:
*/
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
  return addr->address;
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
  /*printf("---------DEBUG SPLITTING---------\n");*/
  /*printf("Data address: %p\n", header->address);*/
  /*printf("Order: %d\n", header->size);*/
  /*printf("Free status: %d\n", header->is_free);*/
  /*printf("Next: %p\n", header->next);*/
  /*printf("------------------------------------\n");*/

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

  /*printf("---------DEBUG NEW NODE ADDED---------\n");*/
  /*printf("Data address: %p\n", new->address);*/
  /*printf("Order: %d\n", new->size);*/
  /*printf("Free status: %d\n", new->is_free);*/
  /*printf("Next: %p\n", new->next);*/
  /*printf("------------------------------------\n");*/
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



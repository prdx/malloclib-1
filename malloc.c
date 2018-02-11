/* 
 Author: Anak Agung Ngurah Bagus Trihatmaja
 Malloc library using buddy allocation algorithm
*/
#include "mallutl.h" /* for data structure */
#include "malloc.h"
#include <math.h>

block_header_t *find_suitable_space(size_t);
void* request_memory(size_t);
void fill_header(block_header_t*, size_t);
size_t upper_power_of_two(size_t);
void push(block_header_t*);
int is_need_split(block_header_t*, size_t);
void split(block_header_t*, size_t);

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
block_header_t *head = NULL;
block_header_t *tail = NULL;

/*------------MALLOC---------------*/
void *malloc(size_t size) {
  // If request is 0, we return NULL
  if(size == 0) return NULL;
  // If request is smaller than 8, we round it to 8
  if(size < 8) size = 8;

  // Round request to the nearest power of 2
  size_t total_size  = sizeof(block_header_t) + size;
  total_size = upper_power_of_two(total_size);

  // Skip this part if there is empty space for request below 4096
  block_header_t* empty_block;
  void* block;
  if(total_size > HEAP_PAGE_SIZE || (empty_block = find_suitable_space(total_size)) == NULL) {
    // Request memory to the OS
    if((block = request_memory(total_size)) == NULL) {
      MALLOC_FAILURE_ACTION;
      return NULL;
    }

    fill_header(block, total_size);
    if(total_size <= HEAP_PAGE_SIZE) {
      empty_block = block;
      empty_block->order = MAX_ORDER;
      empty_block->size = HEAP_PAGE_SIZE;
    }
    // Link the new added node to the list
    // FIXME: Skip link if using mmap, because it will cause bug
    if(total_size <= HEAP_PAGE_SIZE) {
      push(block);
    }
  }

  if(total_size <= HEAP_PAGE_SIZE && empty_block != NULL) {
    // Check if split is necessary, if yes, perform split
    block = empty_block;
    if(is_need_split(block, size) == 1) {
      // Split according buddy allocation
      split(block, total_size);
    }
  }

  // Change the status
  block_header_t* temp = block;
  temp->is_free = occupied;

  
  // Return the address of the data section
  return (char*)block + sizeof(block_header_t);
}

// Request memory to the OS
void* request_memory(size_t size) {
  void* block;

  // Allocate the memory
  if(size <= HEAP_PAGE_SIZE) {
    if ((block = sbrk(HEAP_PAGE_SIZE)) == (void *)-1) {
      MALLOC_FAILURE_ACTION; 
      return NULL;
    }
  }
  else {
    if((block = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == (void*)-1) {
      MALLOC_FAILURE_ACTION;
      return NULL;
    }
  }
  return block;
}

void fill_header(block_header_t *block, size_t size) {
  block->address = (char*)block + sizeof(block_header_t);
  block->is_free = empty;
  block->order = SIZE_TO_ORDER(size);
  block->is_mmaped = size > HEAP_PAGE_SIZE ? mmaped : allocated;
  block->next = NULL;
  // TODO: Fill whether it is left or right
  block->size = size;
}

// Source: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
size_t upper_power_of_two(size_t v) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

block_header_t *find_suitable_space(size_t size) {
  block_header_t *temp =  head;
  while(temp != NULL) {
   // DEBUG
   // char buf[1024];
   // snprintf(buf, 1024, "In find suitable space, at: %p \n", temp);
   // write(STDOUT_FILENO, buf, strlen(buf) + 1);
  
    if(temp->is_free == empty && temp->size >= size) return temp;
    temp = temp->next;
  }
  return NULL;
}

void push(block_header_t *node) {
  // If list is empty, add the new node as head
  if(head == NULL) {
    head = node;
    tail = node;
    return;
  }
  tail->next = node;
  tail = node;
}

int is_need_split(block_header_t *block, size_t size) {
  if(block->size / 2 < size) return 0;
  return 1;
}

void split(block_header_t *block, size_t size) {
  if(block->size / 2 <= size && block->size >= size) return;

  // Fill all the data
  block->order -= 1;
  block->size /= 2;
  void *temp = (char*)block + block->size;
  block_header_t *buddy = (block_header_t*)temp;
  block->position = left;
  fill_header(buddy, block->size);
  buddy->is_mmaped = allocated;
  buddy->is_free = empty;
  buddy->position = right;
  buddy->next = block->next;
  block->next = buddy;

  // Recursively split again if the block still too large
  split(block, size);
}


/* 
 Author: Anak Agung Ngurah Bagus Trihatmaja
 Malloc library using buddy allocation algorithm
*/
#include "mallutl.h" /* for data structure */
#include "malloc.h"
#include <math.h>


/*void *find_suitable_space(size_t);*/
block_header_t* request_memory(size_t);
/*int is_need_split(block_header_t *, size_t);*/
/*void split(block_header_t *, size_t);*/
/*void add_new_header(block_header_t *);*/

pthread_mutex_t global_mutex = PTHREAD_MUTEX_INITIALIZER;
block_header_t *head = NULL;

/*------------MALLOC---------------*/
void *malloc(size_t size) {
  // If request is 0, we return NULL
  if(size == 0) return NULL;
  // If request is smaller than 8, we round it to 8
  if(size < 8) size = 8;
  
  // Request memory to the OS
  // TODO: Skip this part if there is empty space for request below 4096
  // TODO: For request larger than the 4096 always request to the memory for mmap
  block_header_t *block;
  if((block = request_memory(size)) == NULL) {
    MALLOC_FAILURE_ACTION;
    return NULL;
  }

}

// Request memory to the OS
block_header_t* request_memory(size_t size) {
  block_header_t* block;
  size_t total_size  = sizeof(block_header_t) + size;
  // Allocate the memory
  if((block = mmap(NULL, total_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == (void*)-1) {
    return NULL;
  }
  return block;
}

/*void *find_suitable_space(size_t block) {*/
/*}*/

/*int is_need_split(block_header_t *block_header, size_t block) {*/
/*}*/

/*void split(block_header_t *block_header, size_t block) {*/
/*}*/

void add_new_header(block_header_t *header) {
}



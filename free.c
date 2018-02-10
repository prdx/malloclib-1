/*------------FREE---------------*/
#include "mallutl.h" /* for data structure */
#include "free.h"
#include <math.h>

/*void merge_if_possible(header_t*);*/
int disallocate_memory(void*);

void free(void *ptr) {
  // Return directly if address is invalid
  if(ptr == NULL) return;

  // Disallocate memory
  int result;
  if((result = disallocate_memory(ptr)) == -1) return;
}

int disallocate_memory(void* ptr) {
  block_header_t *block =  ptr - sizeof(block_header_t);

  // Check header information, whether it is an mmap or not
  // Munmap
  int result;
  if((result = munmap(block, block->size)) == -1) {
    return -1;
  }
  return 0;
}

/*header_t *get_header(void* ptr) {*/
/*}*/

/*void merge_if_possible(header_t *header) {*/
  /*if(header->next == NULL) {  [> last element <]*/
    /*return;*/
  /*}*/
  /*header_t *current = header;*/

  /*while(1) {*/
    /*if(current == NULL || current->next == NULL) break;*/
    /*if(current->size == current->next->size) {*/
      /*if(current->is_free && current->next->is_free) {*/
        /*current->size += 1;*/
        /*header_t* temp = current->next;*/
        /*current->next = temp->next;*/
        /*temp = NULL;*/
        /*current = header;*/
        /*continue;*/
      /*}*/
      /*current = current->next->next;*/
    /*}*/
    /*else {*/
      /*current = current->next;*/
    /*}*/
  /*}*/
/*}*/

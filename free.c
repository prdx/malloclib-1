/*------------FREE---------------*/
#include "mallutl.h" /* for data structure */
#include "free.h"
#include <math.h>

void merge_if_possible(block_header_t*);
int disallocate_memory(void*);
void remove_node(block_header_t*);

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
  if(block->is_mmaped == mmaped) {
    if((result = munmap(block, block->size)) == -1) {
      return -1;
    }
  }
  else {
    // Change the status to free for request under 4096
    block->is_free = empty;
    merge_if_possible(block);
  }
  return 0;
}

void remove_node(block_header_t* ptr) {
  block_header_t* temp = head, *prev;

  // If head is removed
  if(temp != NULL && temp == ptr) {
    head = temp->next;
    return;
  }

  while(temp != NULL && temp != ptr) {
    prev = temp;
    temp = temp->next;
  }

  // Not found
  if(temp == NULL) return;

  // Unlink the node
  prev->next = temp->next;
}

void merge_if_possible(block_header_t *header) {
  if(header->next == NULL) {      
    return;
  }
  block_header_t *current = header;

  while(1) {
    if(current == NULL || current->next == NULL) break;
    if(current->size == current->next->size) {
      if(current->is_free && current->next->is_free) {
        current->size += 1;
        block_header_t* temp = current->next;
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

#include "mallutl.h" /* for data structure */

header_t *get_header(void*);
void merge_if_possible(header_t*);

void free(void* address) {
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

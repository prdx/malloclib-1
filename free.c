/*------------FREE---------------*/
#include "mallutl.h" /* for data structure */
#include "free.h"
#include <math.h>
#include <sys/mman.h>

void merge_if_possible(header_t*);

void free(void* address) {
  header_t *header = get_header(address);
  if(address == NULL || header == NULL) {
    return;
  }
  
  if(header->size > MAX_ORDER) {
    arena_t *mmaped_arena = (void*)header -  sizeof(arena_t);
    arena_t *arena = arenas;
    if(arena->next != NULL) {
      while(1) {
        if(arena->next == mmaped_arena) {
          arena->next = mmaped_arena->next;
          break;
        }
        arena = arena->next;
      }
    }
    mmaped_arena = NULL;
    munmap(header - sizeof(arena_t), sizeof(arena_t) + sizeof(header_t) + pow(2, header->size));
    return;
  }
  header->is_free = 1;

  arena_t *arena = arenas;
  while (arena != NULL) {
    pthread_mutex_lock(&arena->arena_lock);
    merge_if_possible(arena->base_header); 
    pthread_mutex_unlock(&arena->arena_lock);
    arena = arena->next;
  }
  /*char buf[1024];*/
  /*snprintf(buf, 1024, "%s:%d Debug, address: %p\n",*/
           /*__FILE__, __LINE__, address);*/
  /*write(STDOUT_FILENO, buf, strlen(buf) + 1);*/
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

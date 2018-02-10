/*------------FREE---------------*/
#include "mallutl.h" /* for data structure */
#include "free.h"
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>

void merge_if_possible(header_t*);

void free(void *ptr) {
  if(ptr == NULL || arenas == NULL) return;
  header_t *header = get_header(ptr);
  if(header == NULL) {
    return;
  }
  
  /*pthread_mutex_lock(&global_mutex);*/
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
    /*mmaped_arena = NULL;*/
    munmap(header - sizeof(arena_t), sizeof(arena_t) + sizeof(header_t) + pow(2, header->size));
    char buf[1024];
    snprintf(buf, 1024, "Free: %p\n", ptr);
    write(STDOUT_FILENO, buf, strlen(buf) + 1);
    return;
  }
  header->is_free = 1;

  /*pthread_mutex_lock(&global_mutex);*/
  arena_t *arena = arenas;
  while (arena != NULL) {
    if(arena->allocated == 1) {
      merge_if_possible(arena->base_header);
    }
    arena = arena->next;
  }

  /*pthread_mutex_unlock(&global_mutex);*/
}

header_t *get_header(void* ptr) {
  arena_t *arena = arenas;
  header_t *header = NULL;

  while(arena != NULL) {
    header = arena->base_header;
    while(header != NULL) {
      if(header->address == ptr) {
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

  while(1) {
    if(current == NULL || current->next == NULL) break;
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

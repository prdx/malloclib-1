#include "mallutl.h"

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

void push(block_header_t* node) {
  // If list is empty, add the new node as head
  if (head == NULL) {
    head = node;
    tail = node;
    return;
  }
  tail->next = node;
  tail = node;
}

void remove_node(block_header_t* ptr) {
  block_header_t* temp = head, *prev;

  // If head is removed
  if (temp != NULL && temp == ptr) {
    head = temp->next;
    return;
  }

  while (temp != NULL && temp != ptr) {
    prev = temp;
    temp = temp->next;
  }

  // Not found
  if (temp == NULL)
    return;

  // Unlink the node
  prev->next = temp->next;
}

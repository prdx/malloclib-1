#include "mallutl.h"
#include "memalign.h"
#include <math.h>

void *memalign(size_t alignment, size_t size) {
  // If the alignment is not power of 2, set EINVAL error
  if (ceil(log2(alignment)) != floor(log2(alignment))) {
    errno = EINVAL;
    return NULL;
  }

  if (size == 0) 
    return NULL;

  size_t total_size = sizeof(block_header_t) + size;
  total_size = upper_power_of_two(total_size);

  void *block;
  if ((block = mmap(NULL, total_size, PROT_READ | PROT_WRITE,
          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0)) == (void *)-1) {
    MALLOC_FAILURE_ACTION;
    return NULL;
  }

  void *data_address = (char *)block + sizeof(block_header_t);

  size_t padding = (size_t) data_address % alignment;
  data_address = (char *) data_address - sizeof(block_header_t) + padding;

  block_header_t *header = data_address;
  header->is_mmaped = mmaped;
  header->is_free = occupied;

  header->address = data_address + sizeof(block_header_t);
  header->__padding = padding;
  header->size = total_size;

  return data_address;
}

int posix_memalign(void **memptr, size_t alignment, size_t size) {
  if (alignment % sizeof(void *) != 0) {
    return EINVAL;
  }

  *memptr = memalign(alignment, size);
  return 0;
}

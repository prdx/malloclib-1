#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  size_t size = 12;
  size_t n_size = 20;
  void *mem = malloc(size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
  assert(mem != NULL);
  free(mem);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
  void *mem_c = calloc(size, 8);
  printf("Successfully calloc'd %zu bytes at addr %p\n", size, mem_c);
  assert(mem != NULL);
  mem_c = realloc(mem_c, 20);
  printf("Successfully realloc'd %zu bytes at addr %p\n", n_size, mem_c);
  assert(mem != NULL);
  free(mem_c);
  return 0;
}

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  size_t size = 21472;
  void *mem1 = malloc(size);
  assert(mem1 != NULL);
  void *mem2 = malloc(8);
  assert(mem2 != NULL);
  void *mem3 = malloc(20100);
  assert(mem3 != NULL);
  void *mem4 = malloc(10100);
  assert(mem4 != NULL);
  void *mem5 = malloc(3000);
  assert(mem5 != NULL);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem5);
  return 0;
}

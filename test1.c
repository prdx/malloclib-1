#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  void *mem[10000];
  int i = 0;
  int size = 9;
  for(; i < 10000; i++) {
    mem[i] = malloc(size);
    printf("Malloc: %p\n", mem[i]);
    mem[i] = malloc(size);
    printf("Malloc: %p\n", mem[i]);
    size += 1;
  }

  i = 0;
  for(i = 9999; i >= 0; i--) {
    free(mem[i]);
    printf("Free: %p\n", mem[i]);
  }

  return 0;
}

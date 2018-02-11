# DESIGN OVERVIEW

## Allocating memory

Allocating memory is handled by sbrk and mmap. For request larger than 4096 will be handled by mmap and will not using buddy system.
Meanwhile, for request smaller than 4096, we will use `sbrk` and use buddy system. The reason behind this, is to reduce the operation 
needed when requesting a memory.

## Data structure

I am using list, for the buddy system. In this implementation, we keep the header if someone free the memory, unless merge is possible.
That way, we don't need to create the header again when new request comes and fit the blocks.

Below is the information we keep in the struct:

```
typedef struct __block_header_t {
  void* address;                // the address of the data segment
  unsigned order;               // the order of the size
  size_t size;                  // size of data + size of header
  unsigned is_free;             // flag if data segment is free
  unsigned is_mmaped;           // if we use mmap
  struct __block_header_t *next;// next node
  unsigned __padding;           // bytes for padding
} block_header_t;
```

The header is located right before the data segment.

------------------------------------------------------------
|                 |                     |
|  block header   |     data segment    | .................
|                 |                     |
------------------------------------------------------------

## Code overview

Basically, every functions has its own .c and .h file. The helper codes and data structure definition
is in the `mallutl.c` and `mallutl.h`.

# DESIGN DECISION

## Initial design

The initial design was similar like the current design. However, the difference is in where we allocated the header.

------------------------------------------------------------
|                 |                  |                     |
|  arena header   |   block header   |     data segment    |
|                 |                  |                     |
------------------------------------------------------------

There we have arena header, arena header is basically, a header to manage all of the block headers. It has some statistic on
how many blocks we have, etc. And block header is like our current block header, it manages the segment.
Arena header, block header, and data segment starts with the same position.
We always use fix size (512 * size of block header) to allocate the block header.

In the end, this design consumes more spaces in memory. 
Imagine, even though the user request only one page, the total memory that will be allocated is `sizeof(arena header) + sizeof(block header) * 512 + page needed`.
`512` is for the worst case when every user request smallest possible block only, which means there will be 512 blocks maximum.
So I discard this idea and build the new one from scratch.

In addition, this design is much harder to debug. I had a hard time debugging this design, since there are many possibilities of mistake (wrong address calculation, data structure problems, etc).

## Final design

In the final design, we don't use arena header. The buddy allocation is handled by the block header only. The block header is located right before every data segment and connected to other block header in the linked list.
This way, we can always locate the header by simply substracting the data address by the size of the header.
Comparing to the initial design, in initial design, we have to search one by one in every arena and every header to find which header contains the address. This of course, has unnecessary complexity.

# Known bugs

Known bugs in the code are marked as `// FIXME`.

# TIL

Debugging takes me so long, in the end, I try to implement "TDD" approach. We have the test case, so we can build one function at a time -> run test -> fail -> make it pass -> refactor -> repeat.
Implementing the new design takes me 10 hours with debugging compared to the initial design that takes me around 20 hours with debugging.


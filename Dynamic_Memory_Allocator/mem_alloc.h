#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> 
#include <string.h>   

#define PAGE_SIZE 4096
#define FAILURE -1
#define SUCCESS 0
#define WORD_SIZE 8

typedef struct Header{
    size_t size;
    struct Header * next;
    struct Header * previous;
}Header;

void * mem_alloc(size_t size);
void mem_free(void * mem);

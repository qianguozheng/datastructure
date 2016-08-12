#ifndef __BUFFER_H_
#define __BUFFER_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define BUF_SIZE 2048

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct buffer{
	size_t idx;
	size_t len;
	size_t capacity;
	char *array;
} buffer_t;

int balloc(buffer_t *ptr, size_t capacity);
int brealloc(buffer_t *ptr, size_t len, size_t capacity);
void bfree(buffer_t *ptr);


#define ss_free(ptr)     \
    do {                 \
        free(ptr);       \
        ptr = NULL;      \
    } while(0)

void *ss_malloc(size_t size);

void *ss_realloc(void *ptr, size_t new_size);


#endif

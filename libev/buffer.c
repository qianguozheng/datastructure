#include "buffer.h"

void *ss_malloc(size_t size)
{
    void *tmp = malloc(size);
    if (tmp == NULL)
        exit(EXIT_FAILURE);
    return tmp;
}

void *ss_realloc(void *ptr, size_t new_size)
{
    void *new = realloc(ptr, new_size);
    if (new == NULL) {
        free(ptr);
        ptr = NULL;
        exit(EXIT_FAILURE);
    }
    return new;
}

int balloc(buffer_t *ptr, size_t capacity)
{
	sodium_memzero(ptr, sizeof(buffer_t));
	ptr->array = ss_malloc(capacity);
	ptr->capacity = capacity;
	return capacity;
}

int brealloc(buffer_t *ptr, size_t len, size_t capacity)
{
	if (NULL == ptr)
		return -1;
	size_t real_capacity = max(len, capacity);
	if (ptr->capacity < real_capacity){
		ptr->array = ss_realloc(ptr->array, real_capacity);
		ptr->capacity = real_capacity;
	}
	return real_capacity;
}

void bfree(buffer_t *ptr)
{
	if (NULL == ptr)
		return;
	ptr->idx = 0;
	ptr->len = 0;
	ptr->capacity = 0;
	if (ptr->array != NULL){
		ss_free(ptr->array);
	}
}

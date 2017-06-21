#include <stdio.h>
#include <stdlib.h>

typedef struct list{
	struct list *next;
} list;

void insert(list *l, void *ptr)
{
	list *p = l;
	l = ptr;
	l->next = p;
}
int main()
{
	list root;
	//int i = 10;
	//char c = 'a';
	//double d = 100.00;
	char *i = malloc(100);
	char *c = malloc(100);
	char *d = malloc(100);
	memset(i, 0, 100);
	memset(c, 0, 100);
	memset(d, 0, 100);
	
	memcpy(i, "hello\0", 6);
	memcpy(c, "hell1\0", 6);
	memcpy(d, "hell2\0", 6);
	
	//printf("i=%p,c=%p,d=%p\n", &i, &c, &d);
	
	insert(&root, i);
	insert(&root, c);
	insert(&root, d);
	
	list *l = &root;
	while (l && l->next)
	{
		l = l->next;
		printf("list= %s\n", l);
	}
}

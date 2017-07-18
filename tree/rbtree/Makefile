all:rbtree-data

CFLAGS=-g -O0 -Wall

rbtree-test:rbtree-test.o rbtree.o
rbtree-data: rbtree-data.o rbtree.o

rbtree.o:rbtree.h rbtree.c

rbtree-tst.o:rbtree-data.c

.PHONY:clean

clean:
	rm *.o rbtree-test

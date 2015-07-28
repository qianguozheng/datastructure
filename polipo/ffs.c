/*************************************************************************
	> File Name: ffs.c
	> Author: 
	> Mail: 
	> Created Time: Thu Jul 23 11:36:02 2015
 ************************************************************************/

#include<stdio.h>

#define DEFINE_FFS(type, ffs_name) \
int                         \
ffs_name(type i)            \
{                           \
    int n;                  \
    if (0 == i) return 0;   \
    n = 1;                  \
    while((i & 1) == 0) {   \
        i >>= 1;            \
        n++;                \
    }                       \
    return n;               \
}

#ifndef HAVE_FFS
DEFINE_FFS(int, ffs)
#endif

typedef unsigned int ChunkBitmap;
#define BITMAP_FFS(bitmap) (ffs(bitmap))

#define BITMAP_BIT(i) (((ChunkBitmap)1) << (i))

int get_chunk(ChunkBitmap x)
{
    unsigned i;

    i = BITMAP_FFS(x) - 1;
    printf("i=%u, x=%u\n", i, x);
    x &= ~BITMAP_BIT(i);
    printf("RETURN=%u, i=%u, BITMAP_BIT=%u, ~BITMAP_BIT=%u\n",
          x, i, BITMAP_BIT(i), ~BITMAP_BIT(i));
    return 0;
}

int main(int argc, char *argv[])
{
    ChunkBitmap Xman = atoi(argv[1]);
    get_chunk(Xman);

    return 0;
}

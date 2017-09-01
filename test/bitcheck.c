#include <stdio.h>
#include <stdlib.h>

const char bb_hexdigits_upcase[]  = "0123456789ABCDEF";

/* Emit a string of hex representation of bytes */
char* bin2hex(char *p, const char *cp, int count)
{
        while (count) {
                unsigned char c = *cp++;
                /* put lowercase hex digits */
                *p = 0x20 | bb_hexdigits_upcase[c >> 4];
                printf("p=%s,bb=%c\n", p, bb_hexdigits_upcase[c >> 4]);
                p++;
                
                *p = 0x20 | bb_hexdigits_upcase[c & 0xf];
                printf("p=%s,bb=%c\n", p, bb_hexdigits_upcase[c & 0xf]);
                p++;
                count--;
        }
        return p;
}

int main(){
	char buf[256];
	memset(buf, 0, sizeof(buf));
	bin2hex(buf, , 1);
	
	printf("buf=%s\n", buf);
}

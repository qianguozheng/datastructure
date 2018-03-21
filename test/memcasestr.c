#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline char __ul_cha(char c)
{
    if (isupper(c)) {
        return tolower(c);
    } else if (islower(c)) {
        return toupper(c);
    }    

    return c;
}

void * mem_casestr(const void *s, const char *str, size_t n)
{
    const char *end = (const char *)((char*)s + n);
    const char *ss = (const char *)s;
    const char *tmp = (const char *)s;
    size_t slen = strlen(str); 
    const char *d = str;
    
    while (ss != end) {
		
		if (*d == '\0') {
			printf(" end\n");
            return (void *)(ss - slen);
        }
        
        if (*d != *ss && *d != __ul_cha(*ss)) {
            ss = ++tmp;
            d = str;
            continue;
        }

        ss ++;
        d ++;
        printf("ss=%c, d=%c\n", *ss, *d);
        
        if (*d == '\0') {
			printf(" end---\n");
            return (void *)(ss - slen);
        }
    }
    printf("over\n");

    return NULL;
}


int main(){
	char test[]="www.vivo.com.cn";
	if (mem_casestr(test, "vivo.com.cn", strlen(test))){
		printf("found vivo\n");
	} else {
		printf("no vivo found\n");
	}
}

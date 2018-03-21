#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
	
	char buf[] = "74a52816e85c|115.239.217.181|443|6";
	char *pp = NULL;
	char *p = NULL, *outer=NULL;
	
	pp = buf;
	while((p = strtok_r(pp, "|", &outer)) != NULL) {
		
		printf("p=%s\n", p);
		pp = NULL;
	}
}

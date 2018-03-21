#include <stdio.h>
#include <stdlib.h>


int main(){
	FILE *fp = NULL;
	char buff[4096];
	
	fp = fopen("tuple.list", "r");
	
	if (fp) {
		memset(buff, 0, sizeof(buff));
		
		while(fgets(buff, sizeof(buff), fp) && !feof(fp)) {
			printf("buff(%d)=[%s]\n", strlen(buff), buff);
		}
	}
}

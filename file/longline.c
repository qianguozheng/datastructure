#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	FILE *fp = NULL;
	char buf[10];
	int count = 3;
	
	memset(buf, 0, sizeof(buf));
	fp = fopen("tuple.list", "r");
	char *line = NULL;
	int line_len = 0;
	line = malloc(4096);
	memset(line, 0, 4096);
	if (fp) {
		while(fp && fgets(buf, sizeof(buf), fp)) {
			printf("buf=[%s],len=%d, size=%d\n", buf, strlen(buf), sizeof(buf));
			
			sprintf(line+line_len, "%s", buf);
			line_len += strlen(buf);
			count--;
			if (!count) break;
			
		}
	}
	printf("line[%d]=[%s]\n", line_len, line);
	
	fclose(fp);
}

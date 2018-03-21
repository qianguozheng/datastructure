#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_local_idc_config(char *path){
	FILE *fp = NULL;
	char line[1024];
	char *buf = malloc(1024*16);
	int len = 0;
	
	
	if (NULL == path){
		printf("parameter is NULL");
		goto ERROR;
	}
	
	memset(buf, 0, 1024*16);
	memset(line, 0, sizeof(line));
	fp = fopen(path, "r");
	if (fp) {
		while(!feof(fp) && fgets(line, sizeof(line)-1,fp)){
			//sprintf(buf+len, "%s", line);
			//len += strlen(line);
			//LOG("len=%d\n", len);
			memcpy(buf+len, line, strlen(line));
			len += strlen(line);
			
			memset(line, 0, sizeof(line));
		}
		printf("buf=[%s]\n", buf);
		return buf;
	} else {
		printf("fopen file %s failed\n", path);
	}
ERROR:
	
	if (buf) free(buf);
	return NULL;
}


int main(){
	read_local_idc_config("idc_config.json");
}

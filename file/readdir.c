#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

int readFileList(char *basePath){
	DIR *dir;
	struct dirent *ptr;
	char base[1024];
	
	//printf("in readfile list\n");
	if ((dir = opendir(basePath)) == NULL){
		perror("Open dir error...");
		exit(1);
	}
	//printf("in readfile list 2\n");
	while((ptr = readdir(dir)) != NULL){
		//printf("....\n");
		if (strcmp(ptr->d_name, ".") == 0 || 0 == strcmp(ptr->d_name, ".."))
			continue;
		else if (ptr->d_type == 8)
			printf("d_name=%s/%s\n", basePath, ptr->d_name);
		else if (ptr->d_type == 10)
			printf("d_name=%s/%s\n", basePath, ptr->d_name);
		else if (ptr->d_type == 4)
		{
			//printf("d_name=%s/%s\n", basePath, ptr->d_name);
			#if 1
				memset(base, '\0', sizeof(base));
				strcpy(base, basePath);
				strcat(base, "/");
				strcat(base, ptr->d_name);
				readFileList(base);
			#endif
		}
	}
	closedir(dir);
	return 1;
}

int find_file_name(char *basePath, char *pattern, char *filename){
	DIR *dir;
	struct dirent *ptr;
	char base[1024];
	
	if (!basePath || !pattern){
		printf("paramter is null\n");
		return -1;
	}
	
	//printf("in readfile list\n");
	if ((dir = opendir(basePath)) == NULL){
		perror("Open dir error...");
		return -2;
	}
	//printf("in readfile list 2\n");
	while((ptr = readdir(dir)) != NULL){
		//printf("....\n");
		if (strcmp(ptr->d_name, ".") == 0 || 0 == strcmp(ptr->d_name, ".."))
			continue;
		else if (ptr->d_type == 8){
			printf("d_name=%s/%s\n", basePath, ptr->d_name);
			//LIST
			if (strstr(ptr->d_name, pattern)){
				//store the filename
				sprintf(filename, "%s/%s", basePath, ptr->d_name);
				printf("$$$$$$$$$$ filename=%s\n", filename);
				return 1;
			}
		}
		else if (ptr->d_type == 10)
			printf("d_name=%s/%s\n", basePath, ptr->d_name);
		else if (ptr->d_type == 4)
		{
			continue;
			#if 0
				memset(base, '\0', sizeof(base));
				strcpy(base, basePath);
				strcat(base, "/");
				strcat(base, ptr->d_name);
				readFileList(base);
			#endif
		}
	}
	closedir(dir);
	return 0;
}

int main()
{
	DIR *dir;
	char basePath[1024];
	
	memset(basePath, '\0', sizeof(basePath));
	getcwd(basePath, 999);
	printf("the current dir is:%s\n", basePath);
	
	memset(basePath, 0, sizeof(basePath));
	strcpy(basePath, "/home/weeds/magicwifi/test/datastructure/file");
	//readFileList(basePath);
	char filename[1024];
	memset(filename, 0, sizeof(filename));
	find_file_name(basePath, "test", filename);
	printf("filename=%s\n", filename);
	return 0;
}

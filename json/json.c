#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

int main()
{
	cJSON *pItem = cJSON_CreateObject();
	cJSON_AddStringToObject(pItem, "name", "hello");
	char *szJSON = cJSON_Print(pItem);
	
	printf("Student: %s\n", szJSON);
	
	char *str = "{\"student\":\"one\"}";
	cJSON *pParsed = cJSON_Parse(str);
	
	char stu[20];
	memset(&stu, 0, sizeof(stu));
	
	printf("Test\n");
	if (strlen(cJSON_GetObjectItem(pParsed, "student")) > 0)
	
		sprintf(stu, "%s", cJSON_GetObjectItem(pParsed, "stude1nt")->valuestring);
	printf("Test2\n");
	
	if (strlen(stu) > 0)
	{
		printf("student=%s\n", stu);
	}
	else
	{
		printf("Error\n");
	}
	cJSON_Delete(pParsed);
	cJSON_Delete(pItem);
	return 0;
}

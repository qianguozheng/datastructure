#include <stdio.h>
#include <stdlib.h>

int main(){
	FILE *fp;
	char buf[100], name[16];
	
	
	memset(buf, 0, sizeof(buf));
	if((fp = fopen("network", "r"))){
		while (fgets(buf, sizeof(buf), fp)){
			if (strstr(buf, "interface")){
				//Find name between''
				char *start = strchr(buf, '\'');
				char *end = strrchr(buf, '\'');
				*end = '\0';
				snprintf(name, sizeof(name)-1, "pppoe-%s", start+1);
				
			}
			
			memset(buf, 0, sizeof(buf));
			memset(name, 0, sizeof(name));
		}
	}
}

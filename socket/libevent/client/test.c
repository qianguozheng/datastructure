#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//system("wlanconfig ath0 list sta");
//system("wlanconfig ath1 list sta");
void read_from_file() {
	FILE *fp = fopen("wista.list", "r");
	char line[1024];
	char mac[18];
	
	memset(mac, 0, sizeof(mac));
	memset(line, 0, sizeof(line));
	
	if (fp) {
		fgets(line, sizeof(line), fp);
		while(fgets(line, sizeof(line), fp)) {
			fprintf(stderr, "%s", line);
			
			sscanf(line, "%s %*s", mac);
			printf("mac=%s\n", mac);
		}
		fclose(fp);
	}
}

int main() {
	read_from_file();
}

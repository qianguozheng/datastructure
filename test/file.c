#include <stdio.h>
#include <stdlib.h>

int main()
{

	FILE *fp = NULL;
	char cmp_vendor[128];
	memset(cmp_vendor, 0, sizeof(cmp_vendor));
	
	fp = fopen("vendorinfo", "r");
	if (fp && fgets(cmp_vendor, 127, fp)) {
		printf("file open ok\n");
		printf("1cmp_vendor=[%s]\n", cmp_vendor);
		char *ptr = cmp_vendor;
		while (*ptr){
			if (*ptr == '\r' || *ptr == '\n'){
				*ptr = '\0';
			}
			ptr++;
		}
		printf("2cmp_vendor=[%s]\n", cmp_vendor);
	}
	else {
		sprintf(cmp_vendor, "%s", "magic-gateway");
	}

	printf("3cmp_vendor=[%s]\n", cmp_vendor);
}

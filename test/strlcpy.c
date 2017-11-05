#include <stdio.h>
#include <stdlib.h>
//#include <sys/_types/_ssize_t.h>
#define ENAMETOOLONG 0x01
//#include <_types.h>
#include <sys/cdefs.h>
//#include <Availability.h>
//#include <sys/_types/_size_t.h>
//#include <sys/_types/_null.h>
int main()
{
	int len;
	char path[100];
	char *homedir ="homedir test";

	len = strlcpy(path, homedir, sizeof(path));
	if (len >= sizeof(path))
		return (ENAMETOOLONG);
		
	printf("len=%d\n", len);
	len = strlcat(path, "/" , sizeof(path));
	if (len >= sizeof(path))
		return (ENAMETOOLONG);
	printf("len=%d\n", len);
		
	len = strlcat(path, ".foorc", sizeof(path));
	if (len >= sizeof(path))
		return (ENAMETOOLONG);
	printf("len=%d\n", len);

	return 0;
}

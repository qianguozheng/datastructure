#include <stdio.h>
#include <time.h>

int main()
{
	time_t timep;
	time(&timep);
	printf("%s", asctime_r(gmtime_r(&timep)));
}

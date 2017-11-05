#include <stdio.h>
#include <unistd.h>

int main(void)
{
	char outbuf[500];
	setbuf(stdout, outbuf);

	puts("This is a test of buffered output");
	puts("This outout will go into outbuf");
	puts("and won't appear until the buffer");
	puts("fills up or we flush the stream");

	//sleep(5);
	//puts(outbuf);

	//sleep(5);
	fflush(stdout);

	return 0;
}

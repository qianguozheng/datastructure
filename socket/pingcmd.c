#include <stdio.h>
#include <stdlib.h>

#if 1
int ping (int cnt, int size, char *ip){
	char cmd[128];
	
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "ping -c %d -s %d %s", cnt, size, ip);
	FILE *fp = popen(cmd, "r");
	char buf[512];
	int count = 0;
	if (fp) {
		fgets(buf, 512, fp);
		memset(buf, 0, sizeof(buf));
		while (fgets(buf, 512, fp) && count < 10) {
			float delay = 0.0;
			if (1 == sscanf(buf, "%*s %*s %*s %*s %*s %*s time=%f %*s", &delay)){
				count++ ;
				printf("delay=%0.3f\n", delay);
			}
		}
		fclose(fp);
	}

	printf("count=%d\n", count);
	return 0;
}
#endif

int main(){
	#if 0
	char *buf = "72 bytes from 111.13.100.91: icmp_seq=1 ttl=54 time=42.5 ms";
	float delay=0.0;
	char tmp[16];
	memset(tmp, 0, sizeof(tmp));
	sscanf(buf, "%*s %*s %*s %*s %*s %*s time=%f %*s", &delay);
	//printf("tmp=%s\n", tmp);
	printf("delay=%.3f\n", delay);
	#endif
	ping();
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define FILENAME "/tmp/ra0.signal"

int readfile() {
	char line[512];
	
	char mac[13];
	int rssi0, rssi1, rssi2;
	char bw[12];
	int mcs;
	int idle;
	unsigned long rate;
	unsigned long tx_packets;
	unsigned long rx_packets;
	unsigned long tx_bytes;
	unsigned long rx_bytes;
	
	FILE *fp = fopen(FILENAME, "r");
	if (!fp){
		printf("open file failed\n");
		return -1;
	}
	
	
	memset(line, 0, sizeof(line));
	memset(bw, 0, sizeof(bw));
	memset(mac, 0, sizeof(mac));
	
	while (!feof(fp) && fgets(line, 512, fp)) 
	{
		while (strlen(line) > 0)
		{
			line[strlen(line)-1] = '\0';  //将'\n'替换位\0
		}
		
		//snprintf(pTempStr, 512, "%02x%02x%02x%02x%02x%02x %-7d%-7d%-7d%-6s%-6d%-7d%-7d%-12ld%-12ld%-12ld%-12ld",
		
		//	printk("\n%-19s%-7s%-7s%-7s%-6s%-6s%-7s%-7s%-12s%-12s%-12s%-12s\n",
		//   "MAC", "RSSI0", "RSSI1", "RSSI2", "BW", "MCS",  "Idle", "Rate",
		//   "TxPackets","RxPackets","TxBytes","RxBytes");
	
		int ret = 0;
		if (12 != (ret = sscanf(line, "%s 0x%x 0x%x 0x%x %s 0x%x 0x%x %lu %lu %lu %lu %lu", 
			mac, &rssi0, &rssi1, &rssi2, 
			bw, &mcs, &idle, 
			&rate, &tx_packets, &rx_packets, &tx_bytes, &rx_bytes))) {
			printf("parse parameter failed, error=%s, ret=%d\n", strerror(errno), ret);
			return -1;
		}
		
		printf("mac=%s, rssi=%d,%d,%d, bw=%s\n",mac, rssi0, rssi1, rssi2, bw);
		printf("mcs=%d, idle=%d, %lu, %lu, %lu, %lu, %lu\n", mcs, idle, rate, tx_packets,
				rx_packets, tx_bytes, rx_bytes);
		
	}
}

int main(int argc, char *argv[]) {
	readfile();
}

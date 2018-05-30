#ifndef _PING_H_
#define _PING_H_

#define TEST_NUM 10
#define PKT_LEN 3

typedef struct {
	int length;
	double avg;
	double min;
	double max;
} icmp_result;

typedef struct {
	//int length; //Packet length
	char terminalMac[13];
	icmp_result icmp[PKT_LEN];
} result_array;

result_array network_delay(char *host);

#endif

#ifndef _DHCPINFO_H_
#define _DHCPINFO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef JSONC
	#include <json.h>
#else
	#include <json-c/json.h>
#endif
#define SUCCESS				0
#define ERROR_EMPTY_BUFFER 	-1
#define ERROR_PARSER_JSON 	-2

#define MAC_LENGTH 18
#define IP_LENGTH 16
#define CMD_LENGTH 32

#define MAX_USER 64

typedef struct dhcp_s {
	char mac[MAC_LENGTH];
	char ip[IP_LENGTH];
} dhcp;

typedef struct {
	char ap_mac[MAC_LENGTH];
	char cmd[CMD_LENGTH];
	dhcp value[MAX_USER];
	int length;
} dhcp_pair;

char *form_response_json(dhcp_pair *pair);
int parse_request_json(char *jsonstr, dhcp_pair *pair);
char *dhcp_leases_get(dhcp_pair *pair);

int parse_response_json(char *jsonstr, dhcp_pair *pair);
char *form_request_json(dhcp_pair *pair);

#endif

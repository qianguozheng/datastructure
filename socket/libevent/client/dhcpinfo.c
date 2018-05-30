#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef JSONC
	#include <json.h>
#else
	#include <json-c/json.h>
#endif

#include "dhcpinfo.h"
#include "log.h"

//返回ip地址
char *
dhcp_leases_get(dhcp_pair *pair)
{
	FILE *dhcp_lease = NULL;
	char ip[16];
	char mac[18];
	char name[512] = {'\0'};
	//char * reply = NULL;
	int length = 0, i = 0;
	if (!pair) return NULL;
	
	length = pair->length;

	//printf("length=%d\n", length);
	if (!(dhcp_lease = fopen("/tmp/dhcp.leases", "r"))) {
		return NULL;
	}
	
	//  mac           + ip   +name 
	while (!feof(dhcp_lease) && (fscanf(dhcp_lease, "%*s %17[A-Fa-f0-9:] %15[0-9.] %s %*s", mac, ip, name) == 3)) {
		
		for(i=0; i<length; i++) {
			if (strcmp(pair->value[i].mac, mac) == 0) {
				//printf("ip=%s\n", ip);
				snprintf(pair->value[i].ip, sizeof(pair->value[i].ip), "%s", ip);
				break;
			}
		}
	}

	fclose(dhcp_lease);
	return NULL;
}

//{ap_mac:"", cmd:"request_ip", mac:["", "", ""]}
//{ap_mac:"", cmd:"response_ip", pair: [{"mac":"xxx", "ip":"xxx"}]}

int parse_request_json(char *jsonstr, dhcp_pair *pair) {
	json_object *jso;
	
	if (NULL == jsonstr)
	{
		return ERROR_EMPTY_BUFFER;
	}
	//printf("json=%s\n", jsonstr);
	
	jso = json_tokener_parse(jsonstr);
	
	if (jso)
	{
		json_object *json_cmd;
		if (json_object_object_get_ex(jso, "cmd", &json_cmd))
		{
			const char *cmd = json_object_get_string(json_cmd);
			int len = json_object_get_string_len(json_cmd);
			memcpy(pair->cmd, cmd, len);
			pair->cmd[len] = '\0';
		}
		
		json_object *json_m;
		if (json_object_object_get_ex(jso, "ap_mac", &json_m))
		{
			const char *apmac = json_object_get_string(json_m);
			int len = json_object_get_string_len(json_m);
			memcpy(pair->ap_mac, apmac, len);
			pair->ap_mac[len] = '\0';
		}
		
		json_object *json_mac;
		if (json_object_object_get_ex(jso, "mac", &json_mac))
		{
			enum json_type type = json_object_get_type(json_mac);
			if (type == json_type_array)
			{
				pair->length = json_object_array_length(json_mac);
			}
			else if (type == json_type_object)
			{
				pair->length = 1;
			}
			
			int i;
			for (i=0; i<pair->length; i++)
			{
				json_object *obj = json_object_array_get_idx(json_mac, i);
				//printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
				snprintf(pair->value[i].mac, MAC_LENGTH, "%s", json_object_get_string(obj));//json_object_to_json_string(obj));
			}
		}
	}
	else
	{
		LOG("jsonstr=[%s], not valid\n", jsonstr);
		return ERROR_PARSER_JSON;
	}
	
	json_object_put(jso);
	return SUCCESS;
}

char *form_response_json(dhcp_pair *pair){
	json_object *jso;
	json_object *mac;
	char *p;
	int i = 0;
	
	jso = json_object_new_object();
	mac = json_object_new_array();

	json_object_object_add(jso, "ap_mac", json_object_new_string(pair->ap_mac));
	json_object_object_add(jso, "cmd", json_object_new_string(pair->cmd));

	int pair_num=0;
	for(i = 0; i < pair->length; i++) {
		pair_num++;
		json_object *l = json_object_new_object();
		
		json_object_object_add(l, "mac", json_object_new_string(pair->value[i].mac));
		json_object_object_add(l, "ip", json_object_new_string(pair->value[i].ip));

		json_object_array_add(mac, l);
		
		//printf("[%d]=%s\n", i, json_object_to_json_string(mac));
	}
	if (pair_num > 0)
	{
		json_object_object_add(jso, "pair", mac);
	}
	else 
	{
		json_object_put(mac);
	}
	
	p = strdup(json_object_to_json_string(jso));
	
	json_object_put(jso);
	
	return p;
}

/// Client Usage: get the response from server
char *form_request_json(dhcp_pair *pair) {
	json_object *jso;
	json_object *mac;
	char *p;
	int i = 0;
	
	jso = json_object_new_object();
	mac = json_object_new_array();

	json_object_object_add(jso, "ap_mac", json_object_new_string(pair->ap_mac));
	json_object_object_add(jso, "cmd", json_object_new_string(pair->cmd));

	int pair_num=0;
	//printf("lenght=%d\n", pair->length);
	for(i = 0; i < pair->length; i++) {
		pair_num++;
		//printf("mac=%s\n", pair->value[i].mac);
		json_object_array_add(mac, json_object_new_string(pair->value[i].mac));
	}
	if (pair_num > 0)
	{
		json_object_object_add(jso, "mac", mac);
	}
	else 
	{
		json_object_put(mac);
	}
	
	p = strdup(json_object_to_json_string(jso));
	
	json_object_put(jso);
	
	return p;
}

int parse_response_json(char *jsonstr, dhcp_pair *pair){
	json_object *jso;
	
	if (NULL == jsonstr)
	{
		return ERROR_EMPTY_BUFFER;
	}
	//printf("json=%s\n", jsonstr);
	
	jso = json_tokener_parse(jsonstr);
	
	if (jso)
	{
		json_object *json_cmd;
		if (json_object_object_get_ex(jso, "cmd", &json_cmd))
		{
			const char *cmd = json_object_get_string(json_cmd);
			int len = json_object_get_string_len(json_cmd);
			memcpy(pair->cmd, cmd, len);
			pair->cmd[len] = '\0';
		}
		
		json_object *json_m;
		if (json_object_object_get_ex(jso, "ap_mac", &json_m))
		{
			const char *apmac = json_object_get_string(json_m);
			int len = json_object_get_string_len(json_m);
			memcpy(pair->ap_mac, apmac, len);
			pair->ap_mac[len] = '\0';
		}
		
		json_object *json_mac;
		if (json_object_object_get_ex(jso, "pair", &json_mac))
		{
			enum json_type type = json_object_get_type(json_mac);
			if (type == json_type_array)
			{
				pair->length = json_object_array_length(json_mac);
			}
			else if (type == json_type_object)
			{
				pair->length = 1;
			}
			
			int i;
			for (i=0; i<pair->length; i++)
			{
				json_object *obj = json_object_array_get_idx(json_mac, i);
				//printf("\t[%d]=%s\n", i, json_object_to_json_string(obj));
				//snprintf(pair->value[i].mac, MAC_LENGTH, "%s", json_object_to_json_string(obj));
				json_object *json_mm;
				if (json_object_object_get_ex(obj, "mac", &json_mm))
				{
					const char *mac = json_object_get_string(json_mm);
					snprintf(pair->value[i].mac, MAC_LENGTH,"%s", mac);
				}
				
				json_object *json_ip;
				if (json_object_object_get_ex(obj, "ip", &json_ip))
				{
					const char *ip = json_object_get_string(json_ip);
					snprintf(pair->value[i].ip, IP_LENGTH,"%s", ip);
				}
			}
		}
	}
	else
	{
		LOG("jsonstr=[%s], not valid\n", jsonstr);
		return ERROR_PARSER_JSON;
	}
	
	json_object_put(jso);
	return SUCCESS;
}



#if 0
int main(int argc, char *argv[]) {
	dhcp_pair *pair;
	pair = (dhcp_pair *)malloc(sizeof(dhcp_pair));
	memset(pair, 0, sizeof(dhcp_pair));
	pair->length = 4;
	sprintf(pair->value[0].mac, "%s", "18:c8:e7:80:c1:12");
	sprintf(pair->value[1].mac, "%s", "cc:08:8d:5a:d1:07");
	sprintf(pair->value[2].mac, "%s", "18:c8:e7:80:bb:c3");
	sprintf(pair->value[3].mac, "%s", "54:5a:a6:88:65:c1");
	dhcp_leases_get(pair);
	for (int i=0; i<4; i++)
		printf("pair=%s, %s\n", pair->value[i].mac, pair->value[i].ip);
		
	//json
	char *p = form_json_response(pair);
	printf("p=%s\n", p);
	
	memset(pair, 0, sizeof(dhcp_pair));
	parse_response_json(p, pair);
	printf("length=%d\n", pair->length);
	printf("cmd=%s\n", pair->cmd);
	for(int i=0; i<pair->length; i++) {
		printf("mac[%d]=%s\n", i, pair->value[i].mac);
		printf(" ip[%d]=%s\n", i, pair->value[i].ip);
	}
	free(p);
	
	printf("\n\n========================================\n\n");
	p = form_json_request(pair);
	
	printf("Json Request:\n %s\n\n", p);
	memset(pair, 0, sizeof(dhcp_pair));
	parse_request_json(p, pair);
	
	printf("length=%d\n", pair->length);
	printf("cmd=%s\n", pair->cmd);
	for(int i=0; i<pair->length; i++) {
		printf("mac[%d]=%s\n", i, pair->value[i].mac);
		printf(" ip[%d]=%s\n", i, pair->value[i].ip);
	}
	//parse_request_json("{\"mac\":[\"aaaa\",\"aaaaddd\", \"aaaaccc\", \"aaaaddd\"]}", pair);
}
#endif

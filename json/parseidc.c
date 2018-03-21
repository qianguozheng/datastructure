#include <stdio.h>
#include <string.h>
#ifdef JSONC
	#include <json.h>
#else
	#include <json-c/json.h>
#endif

char json[] = " \
{ \
    \"nodes\": [ \
        {  \
            \"name\": \"Xinjiang\", \
            \"weight\": 1,  \
            \"ips\": [  \
                \"121.201.55.182\"  \
            ],  \
            \"services\": [  \
                {  \
                    \"domain\": \"magicwifi.com.cn\"  \
                },  \
                {  \
                    \"domain\": \"r.magicwifi.com.cn\" \
                }, \
                { \
                    \"domain\": \"www.magicwifi.com.cn\" \
                } \
            ] \
        } \
    ] \
}";

char jsonstr[] = " \
{ \
  \"code\": \"200\", \
  \"msg\": \"success\", \
  \"data\": [ \
    { \
      \"name\": \"zhongshan_bgp\", \
      \"weight\": 1, \
      \"ips\": [ \
        \"121.201.55.180\" \
      ], \
      \"services\": [ \
        { \
          \"domain\": \"magicwifi.com.cn\", \
          \"ip\": \"121.201.55.180\" \
        }, \
        { \
          \"domain\": \"r.magicwifi.com.cn\", \
          \"ip\": \"121.201.55.180\" \
        }, \
        { \
          \"domain\": \"www.magicwifi.com.cn\", \
          \"ip\": \"121.201.55.180\" \
        } \
      ] \
    }, \
    { \
      \"name\": \"zhongshan_mobile\", \
      \"weight\": 1, \
      \"ips\": [ \
        \"183.232.2.4\" \
      ], \
      \"services\": [ \
        { \
          \"domain\": \"magicwifi.com.cn\", \
          \"ip\": \"183.232.2.4\" \
        }, \
        { \
          \"domain\": \"r.magicwifi.com.cn\", \
          \"ip\": \"183.232.2.4\" \
        }, \
        { \
          \"domain\": \"www.magicwifi.com.cn\", \
          \"ip\": \"183.232.2.4\" \
        } \
      ] \
    }, \
    { \
      \"name\": \"zhengjiang_bgp\", \
      \"weight\": 1, \
      \"ips\": [ \
        \"112.73.85.203\" \
      ], \
      \"services\": [ \
        { \
          \"domain\": \"magicwifi.com.cn\", \
          \"ip\": \"112.73.85.203\" \
        }, \
        { \
          \"domain\": \"r.magicwifi.com.cn\", \
          \"ip\": \"112.73.85.203\" \
        }, \
        { \
          \"domain\": \"www.magicwifi.com.cn\", \
          \"ip\": \"112.73.85.203\" \
        } \
      ] \
    }, \
    { \
      \"name\": \"beijing_bgp\", \
      \"weight\": 1, \
      \"ips\": [ \
        \"123.59.251.252\" \
      ], \
      \"services\": [ \
        { \
          \"domain\": \"magicwifi.com.cn\", \
          \"ip\": \"123.59.251.252\" \
        }, \
        { \
          \"domain\": \"r.magicwifi.com.cn\", \
          \"ip\": \"123.59.251.252\" \
        }, \
        { \
          \"domain\": \"www.magicwifi.com.cn\", \
          \"ip\": \"123.59.251.252\" \
        } \
      ] \
    } \
  ] \
}";
typedef struct {
	char ip[16];
	//Node *next;
}IPs;
typedef struct {
	char domain[256];
	char ip[16];
	//Service *next;
}Service;

typedef struct {
	//Data
	int weight;
	int ips_num;
	int service_num;
	IPs ips[4];
	Service services[32];
}Node;

typedef struct {
	char code[8];
	char msg[16];
	Node node[0];
}IDC;

#define SUCCESS				0
#define ERROR_EMPTY_BUFFER 	-1
#define ERROR_PARSER_JSON 	-2

int parse_idc_json(char *jsonstr, IDC *idc)
{
	json_object *jso;
	
	if (NULL == jsonstr)
	{
		return ERROR_EMPTY_BUFFER;
	}
	printf("json=%s\n", jsonstr);
	
	jso = json_tokener_parse(jsonstr);
	
	int length = 0;
	if (jso)
	{
		json_object *json_msg;
		if (json_object_object_get_ex(jso, "msg", &json_msg))
		{
			const char *msg = json_object_get_string(json_msg);
			//int len = json_object_get_string_len(json_msg);
			sprintf(idc->msg, "%s", msg);
		}
		
		json_object *json_code;
		if (json_object_object_get_ex(jso, "code", &json_code))
		{
			const char *code = json_object_get_string(json_code);
			//int len = json_object_get_string_len(json_code);
			sprintf(idc->code, "%s", code);
		}
		
		json_object *json_nodes;
		if (json_object_object_get_ex(jso, "data", &json_nodes))
		{
			enum json_type type = json_object_get_type(json_nodes);
			if (type == json_type_array)
			{
				length = json_object_array_length(json_nodes);
			}
			else if (type == json_type_object)
			{
				length = 1;
			}
			printf("length=%d\n", length);
			int i;
			for (i=0; i < length; i++)
			{
				json_object *obj = json_object_array_get_idx(json_nodes, i);
				
				json_object *json_weight;
				if (json_object_object_get_ex(obj, "weight", &json_weight))
				{
					int weight = json_object_get_int(json_weight);
					printf("weight=%d\n", weight);
					idc->node[i].weight = weight;
				}
				
				json_object *json_ips;
				if (json_object_object_get_ex(obj, "ips", &json_ips))
				{
					int len = 0;
					enum json_type type = json_object_get_type(json_ips);
					if (type == json_type_array)
					{
						len = json_object_array_length(json_ips);
						int j = 0;
						for (j = 0; j < len; j++){
							struct json_object *obj = json_object_array_get_idx(json_ips, j);
							printf("[%d]=%s\n", j, json_object_to_json_string(obj));
      
							sprintf(idc->node[i].ips[j].ip, "%s", json_object_to_json_string(obj));
						}
						idc->node[i].ips_num = len;
					}
					else if (type == json_type_object)
					{
						const char *ip = json_object_get_string(json_ips);
						//int len = json_object_get_string_len(json_ips);
						printf("object ips: ip=%s\n", ip);
						sprintf(idc->node[i].ips[0].ip, "%s", ip);
						idc->node[i].ips_num = 1;
					}
				}
				
				json_object *json_service;
				if (json_object_object_get_ex(obj, "services", &json_service))
				{
					int len = 0, k = 0;
					enum json_type type = json_object_get_type(json_service);
					if (type == json_type_array)
					{
						len = json_object_array_length(json_service);
					}
					else if (type == json_type_object)
					{
						len = 1;
					}
					printf("service len=%d\n", len);
					idc->node[i].service_num = len;
					printf("idc->node[%d].service_num=%d\n", i, idc->node[i].service_num);
					for (k=0; k < len; k++)
					{
						json_object *obj = json_object_array_get_idx(json_service, k);
						
						json_object *json_domain;
						if (json_object_object_get_ex(obj, "domain", &json_domain))
						{
							//int domain_len = json_object_get_string_len(json_domain);
							const char *domain = json_object_get_string(json_domain);
							printf("domain=%s\n", domain);
							sprintf(idc->node[i].services[k].domain, "%s", domain);
							printf("idc->node[%d].services[%d].domain=%s\n", i, k,idc->node[i].services[k].domain);
						}
						
						json_object *json_ip;
						if (json_object_object_get_ex(obj, "ip", &json_ip)){
							//int ip_len = json_object_get_string_len(json_ip);
							const char *ip = json_object_get_string(json_ip);
							printf("ip=%s\n", ip);
							sprintf(idc->node[i].services[k].ip, "%s", ip);
							printf("idc->node[%d].services[%d].ip=%s\n", i, k,idc->node[i].services[k].ip);
						}
					}
				}
			}
		}
	}
	else
	{
		printf("jsonstr=[%s], not valid\n", jsonstr);
		return ERROR_PARSER_JSON;
	}
	
	json_object_put(jso);
	return SUCCESS;
}
#include <sys/stat.h>
int get_file_size(char *filename)
{
	struct stat buf;
	stat(filename, &buf);
	return buf.st_size;
}

#define LOG printf
#define MAXDNAME 1024
int modify_hostsfile(char *filename, Service *pair, int size){
	int file_size = get_file_size(filename);
	int buff_len = file_size + 16 + MAXDNAME*size;
	char *buff = malloc(buff_len), line[1024];
	char *ptr = NULL;
	int len = 0, *found = NULL;
	
	if (file_size <= 0){
		LOG("Service pair is empty\n");
		return 0;
	}
	
	found = malloc(size*sizeof(int));
	
	memset(found, 0, size*sizeof(int));
	memset(buff, 0, buff_len);
	memset(line, 0, sizeof(line));
	//memset(desired_domain, 0, sizeof(desired_domain));
	
	
	
	FILE *fp = fopen(filename, "r+");
	if (!fp){
		LOG("open file failed %s\n", filename);
		return -1;
	}
	
	int i = 0;
	while(!feof(fp) && fgets(line, sizeof(line)-1, fp)){
		//LOG("line=%s\n", line);
		for (i=0; i< size; i++){
			char desired_domain[2014];
			memset(desired_domain, 0, sizeof(desired_domain));
			sprintf(desired_domain, " %s", pair[i].domain);
			if ((ptr = strstr(line, desired_domain)) != NULL
				&& ( *(ptr+strlen(desired_domain)) == ' '
					|| *(ptr+strlen(desired_domain)) == '\n'
					|| *(ptr+strlen(desired_domain)) == '\r'
					|| *(ptr+strlen(desired_domain)) == '#'
					|| *(ptr+strlen(desired_domain)) == EOF
				)){
				//printf("line=%s\n", line);
				memset(line, 0, sizeof(line));
				sprintf(line, "%s %s\n", pair[i].ip, pair[i].domain);
				found[i] = 1;
			}
		}
		
		memcpy(buff+len, line, strlen(line));
		len += strlen(line);
	}
	
	for (i=0; i< size; i++)
	{
		if (0 == found[i])
		{
			memset(line, 0, sizeof(line));
			sprintf(line, "%s %s\n", pair[i].ip, pair[i].domain);
			memcpy(buff+len, line, strlen(line));
			len += strlen(line);
		}
	}
	
	//printf("rewind the fp\n");
	fseek(fp, 0, SEEK_SET);
	fwrite(buff, len, 1, fp);
	fflush(fp);
	fclose(fp);
	return 0;
}
int main(){
	IDC *idc = malloc(sizeof(IDC) + sizeof(Node)*16);
	memset(idc, 0, sizeof(IDC)+ sizeof(Node)*16);
	parse_idc_json(jsonstr, idc);
	printf("=============================== %d\n", (sizeof(IDC)+ sizeof(Node)*16)/1024);

	printf("service_num=%d, %p\n", idc->node[0].service_num, &idc->node[0].service_num);
	int i = 0, j=0, k=0;
	for (i = 0; i < 16; i++){
		if (idc->node[i].weight == 0){
			continue;
		}
		printf("weight=%d\n", idc->node[i].weight);
		printf("node_num=%d\n", idc->node[i].ips_num);
		for (j=0; j< idc->node[i].ips_num; j++){
			printf("node: ip=%s\n", idc->node[i].ips[j].ip);
		}
		
		printf("service_num=%d, %p\n", idc->node[i].service_num, &idc->node[i].service_num);
		for (k=0; k< idc->node[i].service_num; k++){
			printf("service: domain=%s\n", idc->node[i].services[k].domain);
			printf("service: ip=%s\n", idc->node[i].services[k].ip);
		}
	}
	
	modify_hostsfile("hosts", idc->node[0].services, idc->node[0].service_num);
}

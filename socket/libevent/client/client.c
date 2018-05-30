#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
//#include <uci.h>
#include <sys/stat.h>

#include "log.h"
#include "dhcpinfo.h"
#include "ping.h"

#ifdef JSONC
	#include <json.h>
#else
	#include <json-c/json.h>
#endif

#define PORT 3901

int set_noblock(int fd) {
	int ret = -1;
	//Set no-block mode 
	ret = fcntl(fd, F_SETFL, 
	    fcntl(fd, F_GETFL) | O_NONBLOCK);
	if (-1 == ret) {
		LOG("set fd %d noblock error,%s.", strerror(errno));
		return -1;
	}

	return 0;
}

int set_blocking(int fd){
	int ret = -1;
	//Set no-block mode 
	ret = fcntl(fd, F_SETFL, 
	    fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	if (-1 == ret) {
		LOG("set fd %d noblock error,%s.", strerror(errno));
		return -1;
	}

	return 0;
}

int dnsquery(const char *host, int portnum, char *ip_addr)
{
   struct addrinfo hints, *result, *rp;
   char service[6];
   int retval;
   int fd;
   fd_set wfds;
   struct timeval timeout;

   int connect_failed;
   /*
    * XXX: Initializeing it here is only necessary
    *      because not all situations are properly
    *      covered yet.
    */
   int socket_error = 0;

	sprintf(service, "%d", portnum);
   memset((char *)&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_NUMERICSERV; /* avoid service look-up */
#ifdef AI_ADDRCONFIG
   hints.ai_flags |= AI_ADDRCONFIG;
#endif

   if ((retval = getaddrinfo(host, service, &hints, &result)))
   {
	  LOG("Can not resolve %s: %s\n", host, gai_strerror(retval));
      /* XXX: Should find a better way to propagate this error. */
      errno = EINVAL;

      return(-1);
   }

   for (rp = result; rp != NULL; rp = rp->ai_next)
   {

      retval = getnameinfo(rp->ai_addr, rp->ai_addrlen,
         ip_addr, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
      if (retval)
      {
         LOG("Failed to get the host name from the socket structure: %s\n", gai_strerror(retval));
         continue;
      }

	LOG("Address=%s\n", ip_addr);
	if (0 == strcasecmp("::", ip_addr) ||
		0 == strcasecmp("::1", ip_addr))
	{
		LOG("Address=%s, Local IP Address\n", ip_addr);
		continue;
	}
   }

   freeaddrinfo(result);

   LOG("Connected to %s[%s]:%s.\n", host, ip_addr, service);
   return(fd);

}

char *form_http_header(char *ua, char *host, char *url, int content_length){
	char header[512], *p = NULL;
	memset(header, 0, sizeof(header));
	
	sprintf(header, "POST %s HTTP/1.1\r\n"
					"User-Agent: %s\r\n"
					"Host: %s\r\n"
					"Pragma: no-cache\r\n"
					"Content-Type: application/json\r\n"
					"Content-Length: %d\n"
					"\r\n", url, ua, host, content_length);
	
	p = strdup(header);
	//printf("p=%s\n", p);
	if (!p){
		return NULL;
	}
	return p;
}

// http://magicwifi.com.cn/captive/index.html
int parse_url(char *portalurl, char *domain, int *port, char *url)
{
	char urlstr[256];
	char portstr[10];
	char *start = NULL, *end = NULL;
	int ret = 0;
	
	if (NULL == portalurl){
		LOG("portalurl is null\n");
		return -1;
	}
	
	memset(portstr, 0, sizeof(portstr));
	memset(urlstr, 0, sizeof(urlstr));
	sprintf(urlstr, "%s", portalurl);
	
	start = end = urlstr;
	//end = urlstr;
	printf("start=%s, end=%s\n", start, end);
	if (strstr(start, "http://"))
	{
		start += strlen("http://");
	}
	else {
		//No http header before url
		ret = -1;
		return ret;
	}
	printf("start=%s, end=%s\n", start, end);
	if (end = strchr(start, ':')) //Assign port, not default 80
	{
		memcpy(domain, start, end-start);
		printf("domain=[%s]\n", domain);
		start = end+1;
		end = strchr(start, '/');
		
		memcpy(portstr, start, end-start);
		*port = atoi(portstr);
		printf("port=[%d]\n", *port);
		
		start = end;
		memcpy(url, start, strlen(start));
		printf("url=[%s]\n", url);
	}
	else
	{
		*port = 80;
		end = strchr(start, '/');
		memcpy(domain, start, end-start);
		printf("domain=[%s]\n", domain);
		
		start = end;
		memcpy(url, start, strlen(start));
		printf("url=[%s]\n", url);
	}
	return ret;
}

int connect_to_server(int fd, struct sockaddr_in serv_addr){
	int ret = 0;
	
	if (set_noblock(fd)){
		set_noblock(fd);
	}
	
	ret = -1;
	if ((ret = connect(fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) != 0) {

		if (errno != EINPROGRESS){
			LOG("Connection is under building error=%d, %s\n", errno, strerror(errno));
			goto SEL_ERROR;
		}
		else {
			struct timeval tm = {5, 0};  
			fd_set wset;
			
			FD_ZERO(&wset);
			FD_SET(fd,&wset);
			
			while (1) {
				int res = select(fd+1,NULL,&wset,NULL,&tm);  

				if(res < 0)  
				{
					if(ret == -1 && errno == EINTR)
						continue;
					LOG("network error in connect\n");  
					goto SEL_ERROR;
				}
				else if(res == 0)  
				{  
					LOG("connect time out\n");  
					goto SEL_ERROR;
				}
				else
				{
					if(FD_ISSET(fd,&wset))  
					{
						int err = 0;
						int len = sizeof(err);
						getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&err, (socklen_t *)&len);
						if (0 == err) {
							//LOG("connect succeed.\n");  
							goto SEL_SUCC;
						} else {
							LOG("connect failed due to error %d.\n", err);  
							goto SEL_ERROR;
						}
					}
					else
					{
						LOG("other error when select:%s\n",strerror(errno));  
						goto SEL_ERROR;
					}
				}
			}
		}
	}
	
	
	
SEL_ERROR:
	if(set_blocking(fd)){
		set_blocking(fd);
	}
	return -1;
	
SEL_SUCC:
	if(set_blocking(fd)){
		set_blocking(fd);
	}
	return 0;
}
//请求，响应 分开处理， 独立的发送函数

int send_request(char *ipaddr, int port, char *url, 
				 char *content, int content_length,
				 char *resp, int resp_len){
	int sock;
	struct sockaddr_in addr;
	//LOG("ipaddr=%s\n", ipaddr);
	int count = 0;
	do{
		if (-1 == (sock = socket(AF_INET, SOCK_STREAM, 0))){
			LOG("socket error %d\n", errno);
			return -1;
		}

		memset(&addr,0,sizeof(struct sockaddr_in));
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.s_addr=inet_addr(ipaddr);
		
		if(connect_to_server(sock,addr))
		{
		  LOG("cannot connect to server [%s]\n", ipaddr);
		  //close(sock);
			usleep(500);
			close(sock);
		} else {
			break;
		}
		
	} while( count++ < 2);
	if (count > 2){
		LOG("connect failed\n");
		return -2;
	}
	
	struct timeval send_timeout, recv_timeout;
	send_timeout.tv_sec = 5;
	send_timeout.tv_usec = 0;
	recv_timeout.tv_sec = 5;
	recv_timeout.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&send_timeout, sizeof(struct timeval));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&recv_timeout, sizeof(struct timeval));
	
	
	char *header = form_http_header("MagicWiFi", ipaddr, url, content_length);
	int length = strlen(header) + content_length + 1;
	char *request = malloc(length);
	//printf("request=[%s]\n", request);
	if (NULL == request){
		LOG("form request is NULL\n");
		return -3;
	}
	
	//LOG("header[%d]=[%s]\n", length, header);
	
	memset(request, 0, length);
	memcpy(request, header, strlen(header));
	memcpy(request+strlen(header), content, content_length);
	
	LOG("request(%d)=[%s]\n", length, request);
	
	send(sock, request, strlen(request), 0);
	if (header) free(header);
	if (request) free(request);
	
	char response[4096];
	memset(response, 0, sizeof(response));
	resp_len = read(sock, response, 4096);

	if (strlen(response) > 0){
		//LOG("reponse not contain MagicWiFI\n");
		//sleep(1);
		memcpy(resp, response, resp_len);
	}
	close(sock);
	
	return 0;
}

void read_wlan_client(dhcp_pair *pair){
	FILE *fp = NULL;
	char buff[256];
	char mac[18];
	int i = 0;
	
	memset(buff, 0, sizeof(buff));
	memset(mac, 0, sizeof(mac));
	
	fp = fopen("/tmp/wista.wlan0", "r");
	if (fp) {
		while (fgets(buff, sizeof(buff), fp)) {
			if (sscanf(buff, "%*s %s", mac)) {
				printf("mac=%s\n", mac);
				snprintf(pair->value[i].mac, MAC_LENGTH, "%s", mac);
				pair->length = ++i;
			}
			memset(buff, 0, sizeof(buff));
		}
	}
}
void network_status(){
	dhcp_pair *pair = NULL;
	pair = (dhcp_pair *) malloc(sizeof(dhcp_pair));
	memset(pair, 0, sizeof(dhcp_pair));
	
	// Read from /tmp/wista.txt or from command to get mac link to server
	read_wlan_client(pair);
	
	// Form json to get ip
	char *json = form_request_json(pair);
	
	// Send Request
	char response[4096];
	int resp_len;
	memset(response, 0, sizeof(response));
	
	//send_request("192.168.48.1", PORT, "/dhcplease", json， strlen(json), response, resp_len);
	send_request("127.0.0.1", PORT, "/dhcplease", json, strlen(json), response, resp_len);
	
	//printf("response[%d]=[%s]\n", resp_len, response);
	char *p = strstr(response, "\r\n\r\n");
	if (p) {
		parse_response_json(p+strlen("\r\n\r\n"), pair);
	}
	
	result_array ret[64];
	memset(&ret, 0, sizeof(ret));
	
	int i=0;
	for(i=0; i<pair->length; i++) {
		//printf("mac[%d]=%s\n", i, pair->value[i].mac);
		//printf(" ip[%d]=%s\n", i, pair->value[i].ip);
		ret[i] = network_delay(pair->value[i].ip);
		int j=0;
		for (j=0; j< PKT_LEN; j++)
		printf("ip[%s]-max[%f]-min[%f]-avg[%f]\n", pair->value[i].ip,
			ret[i].icmp[j].max, ret[i].icmp[j].min, ret[i].icmp[j].avg);
	}
	
	//TODO: No json defined
	//Form json post to magicwifi
	//Send request to magicwifi
}

void main_loop(){
	
	do {
		network_status();
		sleep(300);
	} while(1);
	
}

int main(int argc, char *argv[]) {
	LOG_INIT("/tmp/delay_stat.log");
	
	main_loop();
	
	LOG_CLOSE();
}

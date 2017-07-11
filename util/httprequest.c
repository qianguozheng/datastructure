#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

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
	  printf("Can not resolve %s: %s", host, gai_strerror(retval));
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
         printf("Failed to get the host name from the socket structure: %s", gai_strerror(retval));
         continue;
      }

	printf("Address=%s", ip_addr);
	if (0 == strcasecmp("::", ip_addr) ||
		0 == strcasecmp("::1", ip_addr))
	{
		printf("Address=%s, Local IP Address", ip_addr);
		continue;
	}
   }



   freeaddrinfo(result);

   printf("Connected to %s[%s]:%s.\n", host, ip_addr, service);
   return(fd);

}

char *form_http_header(char *ua, char *host, char *url){
	char header[512], *p = NULL;
	memset(header, 0, sizeof(header));
	
	sprintf(header, "GET %s HTTP/1.1\r\n"
					"User-Agent: %s\r\n"
					"Host: %s\r\n"
					"Pragma: no-cache\r\n"
					"\r\n", url, ua, host);
	
	p = strdup(header);
	printf("p=%s\n", p);
	if (!p){
		return NULL;
	}
	return p;
}

// http://magicwifi.com.cn/captive/index.html
int parse_url(char *domain, int *port, char *url)
{
	char urlstr[256];
	char portstr[10];
	char *start = NULL, *end = NULL;
	int ret = 0;
	memset(portstr, 0, sizeof(portstr));
	memset(urlstr, 0, sizeof(urlstr));
	strcpy(urlstr, "http://magicwifi.com.cn:38001/captive/index.html");  //Read from config
	
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

int send_request(char *domain, int port, char *url){
	int sock;
	struct sockaddr_in addr;
	char ipaddr[20];
	
	memset(ipaddr, 0, sizeof(ipaddr));
	if (-1 == (sock = socket(AF_INET, SOCK_STREAM, 0))){
		return -1;
	}
	
	dnsquery(domain, 80, ipaddr);
	
	fprintf(stderr, "domain=%s, ipaddr=%s\n", domain, ipaddr);
	memset(&addr,0,sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(port);
	addr.sin_addr.s_addr=inet_addr(ipaddr);

	if(connect(sock,(struct sockaddr *)&addr,sizeof(addr)))
	{
	  printf("cannot connect to server\n");
	  close(sock);
	  return 1;
	}
	
	char *request = form_http_header("MagicWiFi", domain, url);
	printf("request=[%s]\n", request);
	send(sock, request, strlen(request), 0);
	free(request);
	
	
	
	char response[4096];
	memset(response, 0, sizeof(response));
	read(sock, response, 4096);
	
	fprintf(stderr, "response=[%s]\n", response);
	//struct hostent *gethostbyname(const char *hostname);
	close(sock);
	
	if (strstr(response, "200 OK") || strstr(response, "302 Found")
		|| strstr(response, "302 FOUND")
	){
		return 0;
	}
	return -1;
}


int main(int argc,char *argv[])
{
  //char* title;
  //title=getWebPageTitle("www.yahoo.com");
  //printf("Title:%s\n",title);
  
  //
  char domain[128];
  int port;
  char url[128];
  
  memset(domain, 0, sizeof(domain));
  memset(url, 0, sizeof(url));
  
  parse_url(domain, &port, url);
  
  printf("==domain=[%s]\n", domain);
  printf("==url=[%s]\n", url);
  printf("==port=[%d]\n", port);
  
  send_request(domain, port, url);
  
  return 0;
}

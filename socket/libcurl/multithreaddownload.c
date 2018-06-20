#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <evhtp.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>

#include "curl/curl.h"
#include "curl/easy.h"


typedef struct tNode {
	FILE *fp;
	unsigned long startidx;
	unsigned long maxidx;
	void *_curlHeader;
	void *_curl;
	pthread_t _tid;
}tNode;

int bError=0;
int threadCnt=0;
static pthread_mutex_t foo_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t foo1_mutex = PTHREAD_MUTEX_INITIALIZER;
size_t downloadPackage(void *ptr, size_t size, size_t nmemb, void *userdata) {
	tNode *node = (tNode *)userdata;
	size_t written = 0;
	
#if 1
	pthread_mutex_lock(&foo_mutex);
	if (node->startidx + size * nmemb <= node->maxidx) {
		fseek(node->fp, node->startidx, SEEK_SET);
		written = fwrite(ptr, size, nmemb, node->fp);
		node->startidx += size * nmemb;
	} else {
		fseek(node->fp, node->startidx, SEEK_SET);
		written = fwrite(ptr, 1, node->maxidx - node->startidx + 1, node->fp);
		node->startidx += size * nmemb;
	}
	
	pthread_mutex_unlock(&foo_mutex);
#else

	//printf("file startidx=%d\n", node->startidx);
	//Write data to file
	if (node->startidx + size * nmemb <= node->maxidx) {
		fseek(node->fp, node->startidx, SEEK_SET);
		written = fwrite(ptr, size, nmemb, node->fp);
		node->startidx += size * nmemb;
	} else {
		fseek(node->fp, node->startidx, SEEK_SET);
		written = fwrite(ptr, 1, node->maxidx - node->startidx + 1, node->fp);
		node->startidx += size * nmemb;
	}
	
#endif
	//printf("written=%d, size=%d, nmemb=%d\n", written, size, nmemb);
	return written;
}

int assetsManagerProcessFunc(void *ptr, double totalToDownload, double nowDownload, 
		double totalToUpLoad, double nowUpLoaded) {
	static int percent = 0;
	int tmp = 0;
	if (totalToDownload > 0) {
		tmp = (int) (nowDownload / totalToDownload * 100);
	}
	printf("下载进度%0d%%\r", tmp);
	return 0;
}

//获取要下载的远程文件的大小

long getDownloadFileLength(const char *url) {
	double downloadFileLength = 0;
	CURL *handle = curl_easy_init();
	curl_easy_setopt(handle, CURLOPT_URL, url);
	curl_easy_setopt(handle, CURLOPT_HEADER, 1);
	curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
	if (curl_easy_perform(handle) == CURLE_OK) {
		curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &downloadFileLength);
	} else {
		downloadFileLength = -1;
	}
	
	return downloadFileLength;
}


void *workThread(void *pData) {
	tNode *pNode = (tNode *)pData;
	
	int res = 0;
	//do {
	//pthread_mutex_lock(&foo1_mutex);
	res = curl_easy_perform(pNode->_curl);
	//pthread_mutex_unlock(&foo1_mutex);
	if (res != 0) {
		printf("failed %d---\n", res);
		sleep(1);
	}
	
	//} while (res != 0);
	
	curl_slist_free_all(pNode->_curlHeader);
	curl_easy_cleanup(pNode->_curl);
	
	pthread_mutex_lock(&foo_mutex);
	threadCnt--;
	pthread_mutex_unlock(&foo_mutex);
	
	free(pNode);
	pthread_exit(0);
	return NULL;
}

////////////////////////////////////////////////////////////////////////
int parse_url(const char *portalurl, char *domain, int *port, char *url)
{
	char urlstr[256];
	char portstr[10];
	char *start = NULL, *end = NULL;
	int ret = 0;
	
	if (NULL == portalurl){
		printf("portalurl is null\n");
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
		//memcpy(url, start, strlen(start));
		sprintf(url, "%s", start);
		printf("url=[%s]\n", url);
	}
	return ret;
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
	  printf("Can not resolve %s: %s\n", host, gai_strerror(retval));
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
         printf("Failed to get the host name from the socket structure: %s\n", gai_strerror(retval));
         continue;
      }

	//printf("Address=%s\n", ip_addr);
	if (0 == strcasecmp("::", ip_addr) ||
		0 == strcasecmp("::1", ip_addr))
	{
		printf("Address=%s, Local IP Address\n", ip_addr);
		continue;
	}
   }

   freeaddrinfo(result);

   printf("Connected to %s[%s]:%s.\n", host, ip_addr, service);
   return(fd);

}

int replace_domain_with_ip(const char *url, char *returnUrl, char *domain){
	
	char uri[256], ip[16];
	int port = 0;
	
	parse_url(url, domain, &port, uri);
	
	//printf("domain=%s, port=%d, uri=%s\n", domain, port, uri);
	int start = time(NULL);
	dnsquery(domain, 80, ip);
	
	int interval = time(NULL) - start;
	//printf("ip=%s\n", ip);
	sprintf(returnUrl, "http://%s:%d%s", ip, port, uri);
	//printf("returnUrl=%s\n", returnUrl);
	
	return interval;
}
int download(int threadNum, char *iface, char * _packageUrl, char *_storagePath, char *fileName) {
	int start=0, interval=0, end=0;
	
	long fileLength = getDownloadFileLength(_packageUrl);
	if (fileLength <= 0) {
		printf("get the file error...\n");
		return 0;
	}
	
	//Custom Host
	char domain[256], returnUrl[512], httpHost[256];
	memset(domain, 0, sizeof(domain));
	memset(returnUrl, 0, sizeof(returnUrl));
	memset(httpHost, 0, sizeof(httpHost));
	
	interval = replace_domain_with_ip(_packageUrl, returnUrl, domain);
	printf("Parse dns interval %d seconds\n", interval);
	
	start = time(NULL);
	//printf("returnUrl=%s\ndomain=%s\n", returnUrl, domain);
	sprintf(httpHost, "Host: %s", domain);
	
	
	// create a file to save package.
	char outFileName[128];
	sprintf(outFileName, "%s%s", _storagePath, fileName);
	FILE *fp = fopen(outFileName, "wb");
	if (!fp) {
		printf("xxxxxx\n");
		return 0;
	}
	
	curl_global_init(CURL_GLOBAL_ALL);
	
	int gap = fileLength / (threadNum+1);
	//printf("gap=%d, fileLength=%ld, threadNum=%d\n", gap, fileLength, threadNum);
	int i=0;
	for (i=0; i<= threadNum; i++) {
		tNode *pNode = malloc(sizeof(tNode));
		
		if (i< threadNum) {
			pNode->startidx = i *gap;
			pNode->maxidx = (i+1)*gap - 1;
			
		} else {
			//if (fileLength % (threadNum+1) != 0) {
				pNode->startidx = (i) * gap;
				pNode->maxidx = fileLength;
		}
		//printf("i=%d, startidx=%ld, maxidx=%ld\n", i, pNode->startidx, pNode->maxidx);
		
		CURL *_curl = curl_easy_init();
		pNode->_curl = _curl;
		pNode->fp = fp;
		
		//Download package;
		curl_easy_setopt(_curl, CURLOPT_URL, returnUrl);
		curl_easy_setopt(_curl, CURLOPT_INTERFACE, iface);
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, downloadPackage);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, pNode);
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, assetsManagerProcessFunc);
		
		struct curl_slist *headers=NULL; /* init to NULL is important */
		headers = curl_slist_append(headers, httpHost);
		/* pass our list of custom made headers */
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
		pNode->_curlHeader = headers;
		//curl_slist_free_all(headers); /* free the header list */

		 /* get the first 200 bytes */
		char slen[64];
		sprintf(slen, "%ld-%ld", pNode->startidx, pNode->maxidx);
		//printf("slen=%s\n", slen);
		curl_easy_setopt(_curl, CURLOPT_RANGE, slen);
		
		curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
		curl_easy_setopt(_curl, CURLOPT_LOW_SPEED_TIME, 5L);
		
		pthread_mutex_lock(&foo_mutex);
		threadCnt++;
		pthread_mutex_unlock(&foo_mutex);
		
		int rc = pthread_create(&pNode->_tid, NULL, workThread, pNode);
		//free(pNode);
	}
	
	if (bError) {
		fclose(fp);
		return 0;
	}
	while (threadCnt > 0) {
		//printf("threadCnt=\%d\n", threadCnt);
		sleep(1);
	}
	end = time(NULL);
	printf("File Download %d \n", end-start);
	if (end-start > 0){
		printf("Speed %f Mbps\n", 100*8/(double)(end-start));
	}
	
	fclose(fp);
	printf("download succeed...\n");
	return 1;
}

int main(int argc, char *argv[]) {
	int thread_num = 1;
	char url[512];
	char dir[64];
	char file[64];
	char iface[32];
	
	//download(4, "http://cdn-v.magicwifi.com.cn/bitrate_test/file100M.tmp",
	//		"./", "file100M.tmp");
	
	memset(url, 0, sizeof(url));
	memset(dir, 0, sizeof(dir));
	memset(file, 0, sizeof(file));
	memset(iface, 0, sizeof(iface));
	
	int ch;
	while((ch = getopt(argc, argv, "t:u:d:i:f:h::")) != -1) {
		switch (ch) {
		case 'i':
			snprintf(iface, sizeof(iface)-1, "%s", optarg);
			break;
		case 't':
			//printf("t=%s\n", optarg);
			thread_num = atoi(optarg);
			printf("Thread Num   =\t%d\n", thread_num);
			break;
		case 'u':
			snprintf(url, sizeof(url)-1, "%s", optarg);
			break;
		case 'd':
			snprintf(dir, sizeof(dir)-1, "%s", optarg);
			break;
		case 'f':
			snprintf(file, sizeof(file)-1,"%s", optarg);
			break;
		case '?':
			printf("Unknow param [%c]\n", ch);
			//break;
		case 'h':
			printf("Usage: \n-t thread num \n"
				"-u URL\n"
				"-i Interface\n"
				"-d Directory to store downloaded file\n"
				"-f Filename to store\n");
				return 0;
			break;
		default:
			break;
		}
	}
	
	if (strlen(url) == 0) {
		sprintf(url, "%s", "http://cdn-v.magicwifi.com.cn/bitrate_test/file100M.tmp");
	}
	if (strlen(dir) == 0) {
		sprintf(dir, "%s", "./");
	}
	if (strlen(file) == 0) {
		sprintf(file, "%s", "test.bin");
	}
	if (strlen(iface) == 0) {
		sprintf(iface, "%s", "eth1");
	}
	
	printf("Thread Num   =\t%d\n", thread_num);
	printf("Download URL =\t[%s]\n", url);
	printf("Interface    =\t[%s]\n", iface);
	printf("Local Save   =\t[%s%s]\n\n", dir, file);
	
	int start = time(NULL);
	
	
	download(thread_num-1,iface, url, dir, file);
	
	int end = time(NULL);
	int intv = end - start;
	if (intv <= 0) {
		printf("Failed\n");
		return -1;
	}
	printf("%.3f Mbps\n", (100*8)/((double)(end-start)));
	
	
}

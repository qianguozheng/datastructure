#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <pthread.h>

#include "curl/curl.h"
#include "curl/easy.h"


typedef struct tNode {
	FILE *fp;
	unsigned long startidx;
	unsigned long maxidx;
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
	
	curl_easy_cleanup(pNode->_curl);
	
	pthread_mutex_lock(&foo_mutex);
	threadCnt--;
	pthread_mutex_unlock(&foo_mutex);
	
	free(pNode);
	pthread_exit(0);
	return NULL;
}

int download(int threadNum, char * _packageUrl, char *_storagePath, char *fileName) {
	long fileLength = getDownloadFileLength(_packageUrl);
	if (fileLength <= 0) {
		printf("get the file error...\n");
		return 0;
	}
	
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
	printf("gap=%d, fileLength=%ld, threadNum=%d\n", gap, fileLength, threadNum);
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
		printf("i=%d, startidx=%ld, maxidx=%ld\n", i, pNode->startidx, pNode->maxidx);
		
		CURL *_curl = curl_easy_init();
		pNode->_curl = _curl;
		pNode->fp = fp;
		
		//Download package;
		curl_easy_setopt(_curl, CURLOPT_URL, _packageUrl);
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, downloadPackage);
		curl_easy_setopt(_curl, CURLOPT_WRITEDATA, pNode);
		curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0);
		curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, assetsManagerProcessFunc);
		 /* get the first 200 bytes */
		char slen[64];
		sprintf(slen, "%ld-%ld", pNode->startidx, pNode->maxidx);
		printf("slen=%s\n", slen);
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
		printf("threadCnt=\%d\n", threadCnt);
		sleep(1);
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
	
	//download(4, "http://cdn-v.magicwifi.com.cn/bitrate_test/file100M.tmp",
	//		"./", "file100M.tmp");
	
	memset(url, 0, sizeof(url));
	memset(dir, 0, sizeof(dir));
	memset(file, 0, sizeof(file));
	
	int ch;
	while((ch = getopt(argc, argv, "t:u:d:f:h:")) != -1) {
		switch (ch) {
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
			printf("-t thread num \n"
				"-u URL\n"
				"-d Directory to store downloaded file\n"
				"-f Filename to store\n");
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
	
	printf("Thread Num   =\t%d\n", thread_num);
	printf("Download URL =\t[%s]\n", url);
	printf("Local Save   =\t[%s%s]\n\n", dir, file);
	
	
	int start = time(NULL);
	
	
	download(thread_num-1, url, dir, file);
	
	int end = time(NULL);
	int intv = end - start;
	if (intv <= 0) {
		printf("Failed\n");
		return -1;
	}
	printf("%.3f Mbps\n", (100*8)/((double)(end-start)));
	
	
}

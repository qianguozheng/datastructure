#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>

#define FILE_MAX_SIZE 1024*1024*5 //5M

#if 1
#define LOG_INIT(name) log_init(name)
#define LOG(fmt, arg...) logw(fmt, ##arg)
#define LOG_CLOSE() log_close()
#else
#define LOG_INIT(name) fprintf(stderr, "No log file "#name"\n");
#define LOG(fmt, arg...) fprintf(stderr, fmt, arg);
#define LOG_CLOSE() 

#endif


long get_file_size(char *filename){
	long length = 0;
	FILE *fp = NULL;
	
	fp = fopen(filename, "rb");
	if (NULL != fp){
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
	}
	
	if (NULL != fp){
		fclose(fp);
		fp = NULL;
	}
	return length;
}
#if 0
void get_local_time(char *buffer){
	time_t rawtime;
	struct tm* timeinfo;
	
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	sprintf(buffer, "%04d-%02d-%02d %02d:%02d:%02d",
	(timeinfo->tm_year+1900), timeinfo->tm_mon, timeinfo->tm_mday,
	timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec
	);
}


void write_log_file(char *filename, long max_size, char *buffer, unsigned buf_size){
	if (filename != NULL && buffer != NULL){
		long length = get_file_size(filename);
		
		if (length > max_size){
			unlink(filename);
		}
		
		//write log
		FILE *fp;
		fp = fopen(filename, "at+");
		if (NULL != fp){
			char now[32];
			memset(now, 0, sizeof(now));
			get_local_time(now);
			fwrite(now, strlen(now)+1, 1, fp);
			fwrite(buffer, buf_size, 1, fp);
			fclose(fp);
			fp = NULL;
		}
	}
}
#endif
///////////// Design By Weeds guozhengqian0825@126.com ////////////
#define MAX_LOG_BUF 2046
FILE *fp = NULL;
char g_buffer[MAX_LOG_BUF];

int log_init(char *name){
	if (NULL != name){
		long length = get_file_size(name);
		
		if (length > FILE_MAX_SIZE){
			char new_name[256];
			struct timespec ts = {0, 0};
			struct tm vtm;
			
			memset(new_name, 0, sizeof(new_name));
			//gettimeofday(&tv, NULL);
			clock_gettime(CLOCK_REALTIME, &ts);
			localtime_r(&ts.tv_sec, &vtm);
			sprintf(new_name, "%s-%02d%02d%02d%02d%02d%02d", 
					name, vtm.tm_year+1900, vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
			
			rename(name, new_name);
		}
		
		fp = fopen(name, "at+");
		if (NULL != fp){
			fseek(fp, 0, SEEK_END);
			return 0;
		}
	}
	return -1;
}
int log_close(){
	if (fp == NULL)
		return -1;
	fflush(fp);
	fclose(fp);
	fp = NULL;
	return 0;
}

int premakerstr_time(char *buf){
#if 0
	time_t now;
	now = time(&now);
	struct tm vtm;
	localtime_r(&now, &vtm);
	return snprintf(buf, MAX_LOG_BUF, "[%02d-%02d %02d:%02d:%02d] ", 
		vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
#else
	struct timespec ts = {0, 0};
	struct tm vtm;
	//gettimeofday(&tv, NULL);
	clock_gettime(CLOCK_REALTIME, &ts);
	localtime_r(&ts.tv_sec, &vtm);
	return snprintf(buf, MAX_LOG_BUF, "[%02d-%02d %02d:%02d:%02d.%03ld] ", 
		vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec, ts.tv_nsec/1000000);
#endif
}

int logw(char *logformat, ...){
	int size;
	int prestrlen = 0;
	
	char *star = g_buffer;
	prestrlen = premakerstr_time(g_buffer);
	//printf("g_buffer=%s, len=%d\n", g_buffer, prestrlen);
	star += prestrlen;
	
	
	va_list args;
	va_start(args, logformat);
	size = vsnprintf(star, MAX_LOG_BUF-prestrlen, logformat, args);
	va_end(args);
	
	
	if (NULL == fp) {
		fprintf(stderr, "%s", g_buffer);
	}
	else {
		if (1 == fwrite(g_buffer, size+prestrlen, 1, fp)){
			fflush(fp);
		}
		g_buffer[0] = '\0';
	}
	return 0;
}

int main(int argc, char *argv[]){
	//int i;
	//for(i=0; i<10; i++){
	//	char buffer[32];
	//	memset(buffer, 0, sizeof(buffer));
	//	sprintf(buffer, "=====> %d\n", i);
	//	write_log_file("log.txt", FILE_MAX_SIZE, buffer, strlen(buffer));
	//}
	log_init("test.log");
	logw("hello world\n");
	logw("hello worldxxxx, %s, %d, %c\n", "hello", 123, 't');
	log_close();
	
	LOG_INIT("xxx.log");
	LOG("Hello %s World, %d %f\n", "Rich", 123, 123.89);
	LOG_CLOSE();
	return 0;
}

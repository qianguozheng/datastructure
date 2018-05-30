#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <string.h>

#include "log.h"

#if 0
//log filename with xxx.log-2017021345 such as.., no number limit
#define SUFFIX_TIME_FORMAT_LOG 1 
#else
//log with number limit
#define LOG_NUMBER 10
#define SUFFIX_NUMBER_LOG 1
#endif

static long get_file_size(char *filename){
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

///////////// Design By Weeds guozhengqian0825@126.com ////////////
#define MAX_LOG_BUF 2046
FILE *fp = NULL;
char g_buffer[MAX_LOG_BUF];
char g_filename[128] = {'\0'};
int g_rotate = 0;
int g_file_number = 0;

int log_init(char *name){
	if (NULL != name){
		
		if (0 == g_rotate){
			memset(g_filename, 0, sizeof(g_filename));
			sprintf(g_filename, "%s", name);
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

static int premakerstr_time(char *buf){
	struct timespec ts = {0, 0};
	struct tm vtm;
	//gettimeofday(&tv, NULL);
	clock_gettime(CLOCK_REALTIME, &ts);
	localtime_r(&ts.tv_sec, &vtm);
	return snprintf(buf, MAX_LOG_BUF, "[%02d-%02d %02d:%02d:%02d.%03ld] ", 
		vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec, ts.tv_nsec/1000000);
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
		long length = get_file_size(g_filename);
		
		if (length > FILE_MAX_SIZE){
			char new_name[256];
#ifdef SUFFIX_TIME_FORMAT_LOG
			struct timespec ts = {0, 0};
			struct tm vtm;
			
			memset(new_name, 0, sizeof(new_name));
			//gettimeofday(&tv, NULL);
			clock_gettime(CLOCK_REALTIME, &ts);
			localtime_r(&ts.tv_sec, &vtm);
			sprintf(new_name, "%s-%02d%02d%02d%02d%02d%02d", 
					g_filename, vtm.tm_year+1900, vtm.tm_mon+1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
#elif SUFFIX_NUMBER_LOG
			memset(new_name, 0, sizeof(new_name));
			sprintf(new_name, "%s.%d", g_filename, g_file_number++);
			if (LOG_NUMBER <= g_file_number) {
				g_file_number = 0;
			}
			
			if (access(new_name, F_OK) == 0) unlink(new_name);
#else
			//only one file
			memset(new_name, 0, sizeof(new_name));
			sprintf(new_name, "%s", g_filename);
			if (access(new_name, F_OK) == 0) unlink(new_name);
#endif
			//rename(name, new_name);
			fflush(fp);
			fclose(fp);
			
			rename(g_filename, new_name);
			
			g_rotate = 1;
			//open file descriptor
			log_init(g_filename);
		}
		
		if (1 == fwrite(g_buffer, size+prestrlen, 1, fp)){
			fflush(fp);
		}
		g_buffer[0] = '\0';
	}
	return 0;
}

#if 0
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
#endif

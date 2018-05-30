#ifndef _LOG_H_
#define _LOG_H_

#define FILE_MAX_SIZE 1024*1024*5 //5M

#if 1
#define LOG_INIT(name) log_init(name)
#define LOG(fmt, arg...) logw("%s(%d)"fmt, __FUNCTION__,__LINE__,##arg)
#define LOG_CLOSE() log_close()
#else
#define LOG_INIT(name) fprintf(stderr, "No log file "#name"\n");
#define LOG(fmt, arg...) fprintf(stderr, fmt, ##arg);
#define LOG_CLOSE() 

#endif

int log_init(char *name);
int log_close();
int logw(char *logformat, ...);

#endif

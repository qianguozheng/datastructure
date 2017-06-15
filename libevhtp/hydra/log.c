#include <sys/time.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include <event2/util.h>

static FILE *lmfp = NULL;
static off_t log_max_size = 1024*1024;

int log_level = 0;  // 0 : DEBUG, 1:MSG, 2:WARN, 3:ERR 

static char log_desc[5][8] = {
	"debug", "msg", "warn", "err", "over"
};

#include <sys/time.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>
#include <string.h>

#include <event2/util.h>

static inline void cleanup_logfile(void)
{
	struct stat st;
	int mret;
	
	mret = fstat(fileno(lmfp), &st);
	if (mret != 0) return;
	if (st.st_size < log_max_size)  return;

	// truncate /tmp/xxxx.log
	errno = 0;
	rewind(lmfp);
	mret = errno;
	if (errno != 0 || ftruncate(fileno(lmfp), 0) != 0) ;
	fprintf(lmfp, "clear log  %d(%s), %d (%s)\n", mret, strerror(mret), errno, strerror(errno));
	fflush(lmfp);
}

// log callback for libevent ;
// @severity = 0 --3  (0 : DEBUG, 1:MSG, 2:WARN, 3:ERR )
void write_to_file_cb(int severity, const char *msg)
{
	if (severity < log_level) {
		return;
	}

	if (!lmfp) return;
	cleanup_logfile();

	struct timeval cur_tv;
	char tv[32], dt[42];
	gettimeofday(&cur_tv, NULL);
	__time_to_str(tv, sizeof(tv), cur_tv.tv_sec, "%Y-%m-%d %H:%M:%S");
	snprintf(dt, sizeof(dt), "%s.%06u", tv, (unsigned int)(cur_tv.tv_usec));

	fprintf(lmfp, "%s|%s|%s\n", dt, log_desc[severity], msg); 
	fflush(lmfp);
}

// @logname = "/tmp/lmguard00x.log"
int lmg_init_log(const char *logname, off_t max_size)
{
	lmfp = fopen(logname, "a+");
	if (!lmfp) {
		fprintf(stderr, "fopen %s error %s\n", logname, strerror(errno));
		return -1;
	}

	evutil_make_socket_closeonexec(fileno(lmfp));

	log_max_size = max_size;
	return 0;
}

void lmg_release_log(void)
{
	if (!lmfp)  return;
	
	fclose(lmfp);
	lmfp = NULL;
}



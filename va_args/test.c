#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>


#define debug(...) printf(__VA_ARGS__)
#define HAVE_VSYSLOG 1

static int _debug = 1;
static int _syslog = 1;

void mc_debug(const char *msg, ...)
{
  va_list ap;
  if(_debug) {
    va_start(ap, msg);
#if HAVE_VSYSLOG
    if(_syslog) {
		vsyslog(LOG_DEBUG, msg, ap);
	} else
#endif
		vprintf(msg, ap);
    va_end(ap);
  }
}

int main(void)
{
	debug("hello %d, %s, %f\n", 1, "shit", 1.0);
	mc_debug("test %s", "helo world");
}

#ifndef _UTILS_H_
#define _UTILS_H_
#include <stdint.h>

#define IF_SCAN_PATTERN \
	" %[^ :]:%u %u" \
	" %*d %*d %*d %*d %*d %*d" \
	" %u %u"

struct traffic_entry {
	uint32_t time;
	uint32_t rxb;
	uint32_t rxp;
	uint32_t txb;
	uint32_t txp;
};

struct traffic_entry upload_bitrate(char *iface);

#endif


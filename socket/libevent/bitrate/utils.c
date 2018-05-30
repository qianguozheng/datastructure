#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct traffic_entry update_ifstat(
	const char *ifname, uint32_t rxb, uint32_t rxp, uint32_t txb, uint32_t txp
) {
	//char path[1024];

	//struct stat s;
	struct traffic_entry e;
#if 0
	snprintf(path, sizeof(path), DB_IF_FILE, ifname);

	if (stat(path, &s))
	{
		if (init_file(path, sizeof(struct traffic_entry)))
		{
			fprintf(stderr, "Failed to init %s: %s\n",
					path, strerror(errno));

			return -1;
		}
	}
#endif
	e.time = htonl(time(NULL));
	e.rxb  = htonl(rxb);
	e.rxp  = htonl(rxp);
	e.txb  = htonl(txb);
	e.txp  = htonl(txp);

	return e;
}

struct traffic_entry upload_bitrate(char *iface){
	FILE *info;
	uint32_t rxb, txb, rxp, txp;
	uint32_t udp, tcp, other;
	char line[1024];
	char ifname[16];
	struct traffic_entry e;
	memset(&e, 0, sizeof(struct traffic_entry));
	
	if ((info = fopen("/proc/net/dev", "r")) != NULL)
	{
		while (fgets(line, sizeof(line), info))
		{
			if (strchr(line, '|'))
				continue;

			if (sscanf(line, IF_SCAN_PATTERN, ifname, &rxb, &rxp, &txb, &txp))
			{
				if (0 == strncmp(ifname, iface, sizeof(ifname))) {
					//e = update_ifstat(ifname, rxb, rxp, txb, txp);
					e.time = (time(NULL));
					e.rxb  = (rxb);
					e.rxp  = (rxp);
					e.txb  = (txb);
					e.txp  = (txp);
				}
			}
		}

		fclose(info);
	}
	
	return e;
}


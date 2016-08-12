#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int uci_get_config(const char *uci_cmd, char *out, size_t osize)
{
	FILE *fp = NULL;
	int ret = -1;
	*out = 0;
	
	fp = popen(uci_cmd, "r");
	if (!fp) {
		//MLOG(mfp,"uci cmd %s error!", uci_cmd);
		return -1;
	}

	fgets(out, osize-1, fp);
	if (0 != out[0]) {  // not empty
		//out[osize-1] = 0;
		out[strlen(out)-1] = '\0';
		ret = 0;
	}

	pclose(fp);
	return ret;
}

/**
 *  Add by qianguozheng at 2016-5-25 to check whether enable squid or not
 *  Check whether enable proxy or not.
 *  RETURN: 0, disable; 1, enable
 */
static int proxy_switch()
{
	char buf[8];
	int ret, enable;
	memset(buf, 0, sizeof(buf));
	
	ret = uci_get_config("uci -q get comm_cfg.privoxy.enable", buf, sizeof(buf));
	if (0 == ret)
	{
		enable = atoi(buf);
		return enable;
	}
	else
	{
		return 0;
	}
}

int main()
{
	printf("xx=%d\n", proxy_switch());
	return 0;
}

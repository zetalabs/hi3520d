/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_if_ipaddr.c
 *
 * Description  : network interface IP address handler
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-14   0.1       Write it from scrach
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
        } \
} while(0)
#else
#define DBG(format, args...)
#endif

bool ezcfg_util_if_get_ipaddr(const char ifname[IFNAMSIZ], char ip[INET_ADDRSTRLEN])
{
	int s;
	struct ifconf ifconf;
	struct ifreq ifr[16];
	int ifs;
	int i;

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		return false;
	}

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof(ifr);

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
		return false;
	}
	close(s);

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	for (i = 0; i < ifs; i++) {
		struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;
		if (strcmp(ifname, ifr[i].ifr_name) == 0) {
			if (inet_ntop(AF_INET, &s_in->sin_addr, ip, INET_ADDRSTRLEN) == NULL) {
				return false;
			}
			return true;
		}
	}

	return false;
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_wan_interface.c
 *
 * Description  : ezbox check service status
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-11   0.1       Write it from scratch
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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "ezcd.h"

#if 0
#define DBG printf
#else
#define DBG(format, arg...)
#endif

bool utils_wan_interface_is_up(void)
{
	int wan_type;
	char wan_ifname[IFNAMSIZ];
	int rc;
	int sockfd;
	struct ifreq ifreq;
	int flags;
	struct sockaddr_in *sin;
	uint32_t ipaddr;

	wan_type = utils_get_wan_type();
	if (wan_type == WAN_TYPE_UNKNOWN)
		return false;

	switch (wan_type) {
	case WAN_TYPE_DHCP :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, DHCP_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return false;

                break;

	case WAN_TYPE_STATIC :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return false;

                break;

	case WAN_TYPE_PPPOE :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, STATIC_IFNAME), wan_ifname, sizeof(wan_ifname));
		if (rc < 0)
			return false;

                break;
	}

	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (sockfd < 0)
		return false;

	ifreq.ifr_addr.sa_family = AF_INET;
	strncpy(ifreq.ifr_name, wan_ifname, IFNAMSIZ);
	/* check interface status */
	rc = ioctl(sockfd, SIOCGIFFLAGS, &ifreq);
	if (rc < 0) {
		flags = 0;
	}
	else {
		flags = ifreq.ifr_flags;
	}

	/* check interface IP address */
	rc = ioctl(sockfd, SIOCGIFADDR, &ifreq);
	if (rc < 0) {
		ipaddr = 0;
	}
	else {
		sin = (struct sockaddr_in *) &ifreq.ifr_addr;
		ipaddr = sin->sin_addr.s_addr;
	}

	close(sockfd);

	if (!(flags & IFF_UP) || (ipaddr == 0)) {
		return false;
	}

	return true;
}

bool utils_wan_interface_wait_up(int s)
{
	if (s < 1) {
		do {
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
		} while (utils_wan_interface_is_up() == false);
		return true;
	}
	else {
		do {
			if (utils_wan_interface_is_up() == true) {
				return true;
			}
			DBG("%s(%d) sleep 1 seconds\n", __func__, __LINE__);
			sleep(1);
			s--;
		} while(s > 0);
		return false;
	}
}

bool utils_wan_interface_wait_down(int s)
{
	if (s < 1) {
		do {
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
		} while (utils_wan_interface_is_up() == true);
		return true;
	}
	else {
		do {
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
			s--;
			if (utils_wan_interface_is_up() == false) {
				return true;
			}
		} while(s > 0);
		return false;
	}
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_telnetd.c
 *
 * Description  : ezbox run telnet service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * 2011-10-27   0.2       Modify it to use rcso framework
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

#include "ezcd.h"

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_telnetd(int argc, char **argv)
#endif
{
	int rc;
	int ip[4];
	char buf[256];
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "telnetd")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
	    utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, TELNETD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
	    utils_service_binding_wan(NVRAM_SERVICE_OPTION(RC, TELNETD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
	} else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	rc = sscanf(buf, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (rc != 4) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		utils_system("start-stop-daemon -K -s KILL -n telnetd");
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, TELNETD_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		snprintf(buf, sizeof(buf), "start-stop-daemon -S -n telnetd -a %s -- -l %s -b %d.%d.%d.%d", CMD_TELNETD, CMD_LOGIN, ip[0], ip[1], ip[2], ip[3]);
		utils_system(buf);
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

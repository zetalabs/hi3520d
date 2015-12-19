/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_dnsmasq.c
 *
 * Description  : ezbox run dns & dhcp service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * 2011-10-07   0.2       Modify it to use rcso framework
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
#include "pop_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/tmp/rc_dnsmasq.log", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_dnsmasq(int argc, char **argv)
#endif
{
	int rc;
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "dnsmasq")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
            utils_service_binding_lan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
		/* It's good */
	}
	else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
            utils_service_binding_wan(NVRAM_SERVICE_OPTION(RC, DNSMASQ_BINDING)) == true) {
		/* It's good */
	}
	else
#endif
#if ((HAVE_EZBOX_LAN_NIC == 1) || (HAVE_EZBOX_WAN_NIC == 1))
	{
		return (EXIT_FAILURE);
	}
#endif

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		utils_system("start-stop-daemon -K -n dnsmasq");
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, DNSMASQ_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
		pop_etc_dnsmasq_conf(RC_ACT_START);
		utils_system("start-stop-daemon -S -n dnsmasq -a " CMD_DNSMASQ);
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

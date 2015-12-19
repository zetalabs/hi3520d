/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_upnp_monitor.c
 *
 * Description  : ezbox run upnp_monitor service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-22   0.1       Write it from scratch
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_upnp_monitor(int argc, char **argv)
#endif
{
	int rc = -1;
	char buf[256];
	int ip[4];
	proc_stat_t *pidList;
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "upnp_monitor")) {
		return (EXIT_FAILURE);
	}

	buf[0] = '\0';
#if (HAVE_EZBOX_LAN_NIC == 1)
	if (strcmp(argv[1], "lan") == 0 &&
	    utils_service_binding_lan(NVRAM_SERVICE_OPTION(EZCFG, UPNPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(LAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
	} else
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
	if (strcmp(argv[1], "wan") == 0 &&
	    utils_service_binding_wan(NVRAM_SERVICE_OPTION(EZCFG, UPNPD_BINDING)) == true) {
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WAN, IPADDR), buf, sizeof(buf));
		if (rc < 0) {
			return (EXIT_FAILURE);
		}
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

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* trigger ezcd upnp_monitor stop */
		snprintf(buf, sizeof(buf), "%s upnp monitor stop", CMD_EZCM);
		utils_system(buf);

		/* notify upnp_monitor first */
		pidList = utils_find_pid_by_name("upnp_monitor");
		if (pidList != NULL) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGALRM);
			}
			free(pidList);
		}

		/* wait a second to send out SSDP byebye */
		sleep(1);

		/* stop upnp_monitor */
		pidList = utils_find_pid_by_name("upnp_monitor");
		while (pidList != NULL) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGTERM);
			}
			free(pidList);
			sleep(1);
			pidList = utils_find_pid_by_name("upnp_monitor");
		}

		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */

	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(EZCFG, UPNPD_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		mkdir("/etc/upnp_monitor", 0755);

		/* setup upnp_monitor task file path in ezcd */
		snprintf(buf, sizeof(buf), "%s upnp monitor task_file %s",
			CMD_EZCM, UPNP_MONITOR_TASK_FILE_PATH);
		utils_system(buf);

		/* start upnp_monitor */
		utils_system(CMD_UPNP_MONITOR);

		/* wait upnp_monitor start up */
		while (utils_has_process_by_name("upnp_monitor") == false) {
			sleep(1);
		}

		/* trigger ezcd upnp_monitor start */
		snprintf(buf, sizeof(buf), "%s upnp monitor start", CMD_EZCM);
		utils_system(buf);

		pidList = utils_find_pid_by_name("upnp_monitor");
		if (pidList != NULL) {
			int i;
			for (i = 0; pidList[i].pid > 0; i++) {
				kill(pidList[i].pid, SIGALRM);
			}
			free(pidList);
		}

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_wpa_supplicant.c
 *
 * Description  : ezbox run wpa_supplicant service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-08-04   0.1       Write it from scratch
 * 2011-10-05   0.2       Modify it to use rcso framework
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
#include <net/if.h>

#include "ezcd.h"
#include "pop_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/wpa_supplicant.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_wpa_supplicant(int argc, char **argv)
#endif
{
	char wifi_nic[IFNAMSIZ];
	char cmd[256];
	int rc;
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "wpa_supplicant")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		utils_system("start-stop-daemon -K -n wpa_supplicant");
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, WPA_SUPPLICANT_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(WIFI_LAN, IFNAME), wifi_nic, sizeof(wifi_nic));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		pop_etc_wpa_supplicant_conf(RC_ACT_START);
		snprintf(cmd, sizeof(cmd), "start-stop-daemon -S -n wpa_supplicant -a /usr/sbin/wpa_supplicant -- -B -D%s -i%s -c/etc/wpa_supplicant-%s.conf", "nl80211", wifi_nic, wifi_nic);
#if 0
		utils_system("start-stop-daemon -S -n wpa_supplicant -a /usr/sbin/wpa_supplicant -- -i wlan0 -c /etc/wpa_supplicant-wlan0.conf");
#else
		utils_system(cmd);
#endif
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

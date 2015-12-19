/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_realtime.c
 *
 * Description  : ezbox run realtime RTAI service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-08   0.1       Write it from scratch
 * 2011-10-28   0.2       Modify it to use rcso framework
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
	FILE *fp = fopen("/tmp/realtime.debug", "a"); \
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
int rc_realtime(int argc, char **argv)
#endif
{
	int rc;
	char buf[128];
	char cmd[128];
	char modpath[128];
	char *p, *q, *r;
	FILE *file;
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "realtime")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* stop realtime RTAI service */
		/* get the kernel module name for realtime */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODULES), buf, sizeof(buf));
		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				/* split modules */
				q = strrchr(p, ',');
				if (q != NULL) {
					*q = '\0';
					p = q+1;
				}

				/* strip '/' in module path */
				r = strrchr(p, '/');
				if (r != NULL) {
					p = r+1;
				}

				/* rmmod kernel module */
				snprintf(cmd, sizeof(cmd), "%s %s %s",
					"/usr/bin/emc_module_helper", "remove", p);
				utils_system(cmd);
				DBG("cmd=[%s]\n", cmd);

				if (q != NULL)
					p = buf;
				else 
					p = NULL;
			}
		}
		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODPATH), modpath, sizeof(modpath));
		if (rc <= 0) {
			return (EXIT_FAILURE);
		}

		/* start realtime RTAI service */
		/* get the kernel module name for realtime */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, MODULES), buf, sizeof(buf));
		if (rc > 0) {
			p = buf;
			while(p != NULL) {
				q = strchr(p, ',');
				if (q != NULL)
					*q = '\0';

				/* insmod kernel module */
				snprintf(cmd, sizeof(cmd), "%s %s %s/%s.ko",
					"/usr/bin/emc_module_helper", "insert", modpath, p);
				utils_system(cmd);
				DBG("cmd=[%s]\n", cmd);

				if (q != NULL)
					p = q+1;
				else
					p = NULL;
			}
		}
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EMC2, RTAPI_DEBUG), buf, sizeof(buf));
		if (rc > 0) {
			file = fopen("/proc/rtapi/debug", "w");
			if (file != NULL) {
				/* set debug mode */
				fprintf(file, "%s", buf);
				fclose(file);
			}
		}
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

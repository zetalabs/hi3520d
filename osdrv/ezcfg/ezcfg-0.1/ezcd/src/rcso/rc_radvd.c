/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_radvd.c
 *
 * Description  : ezbox run radvd Linux IPv6 Router Advertisement Daemon service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-08-21   0.1       Write it from scratch
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
#include "rc_func.h"

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/tmp/radvd.debug", "a"); \
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
int rc_radvd(int argc, char **argv)
#endif
{
	int rc;
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "radvd")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* make sure the radvd service has been started */
		if (flag == RC_ACT_STOP) {
			utils_system("start-stop-daemon -K -n radvd");
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, RADVD_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* make sure the radvd service has not been started */
		pop_etc_radvd_conf(RC_ACT_START);
		utils_system("start-stop-daemon -S -n radvd -a " CMD_RADVD);
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

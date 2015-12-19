/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_lxc.c
 *
 * Description  : ezbox run LXC Linux Container service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-07-23   0.1       Write it from scratch
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
	FILE *fp = fopen("/tmp/lxc.debug", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static int clean_lxc_nvram(void)
{
	ezcfg_api_nvram_unset(NVRAM_SERVICE_OPTION(RC, LXC_ENABLE));

	return (EXIT_SUCCESS);
}


#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_lxc(int argc, char **argv)
#endif
{
	int rc;
#if 0
	char *p;
	char name[64];
	char buf[64];
	char cmd[256];
	char ini_file[64];
	char ini_dir[64];
	char old_dir[64];
	int num, i;
#endif
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "lxc")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_RESTART :
	case RC_ACT_STOP :
		/* make sure the lxc service has been started */

		if (flag == RC_ACT_STOP) {
			ret = EXIT_SUCCESS;
			break;
		}
		else if (flag == RC_ACT_RELOAD) {
			/* then restore the default lxc settings */
			clean_lxc_nvram();
			ret = EXIT_SUCCESS;
			break;
		}

		/* RC_ACT_RESTART fall through */
		sleep(1);
	case RC_ACT_START :
		rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, LXC_ENABLE), "1");
		if (rc < 0) {
			return (EXIT_FAILURE);
		}

		/* make sure the lxc service has not been started */

		mkdir("/etc/lxc", 0755);
		mkdir("/var/lib/lxc", 0777);

		pop_etc_lxc_lxc_conf(RC_ACT_START);

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_cgroup.c
 *
 * Description  : ezbox env agent runs cgroup service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2013-03-19   0.1       Write it from scratch
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
int env_action_cgroup(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "cgroup")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_START :
		/* generate /etc/cgconfig.conf */
		pop_etc_cgconfig_conf(RC_ACT_START);

		/* generate /etc/cgrules.conf */
		pop_etc_cgrules_conf(RC_ACT_START);

	#if 0
		utils_system(CMD_CGCONFIGPARSER " -l /etc/cgconfig.conf" );
	#endif

		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_ezcm.c
 *
 * Description  : ezbox env agent populate ezcm config file
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
 * 2011-10-22   0.2       Modify it to use rcso framework
 * 2012-12-25   0.3       Modify it to use agent action framework
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
#define DBG printf
#else
#define DBG(format, arg...)
#endif

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_ezcm(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "ezcm")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
#if 0
		/* generate ezcm config file */
		pop_etc_ezcm_conf(flag);
#endif
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_RELOAD :
#if 0
		/* re-generate ezcm config file */
		pop_etc_ezcm_conf(flag);
#endif
		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

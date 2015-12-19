/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_state.c
 *
 * Description  : ezbox env agent populates environment state
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-12-21   0.1       Write it from scratch
 * 2012-12-25   0.2       Modify it to use agent action framework
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
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, arg...)
#endif

static int state_user_defined(int argc, char **argv)
{
	if (argc < 1) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "sync") == 0) {
		if (argc != 2)
			return (EXIT_FAILURE);
		return (EXIT_SUCCESS);
	}
	else {
		return (EXIT_FAILURE);
	}
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_state(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "state")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_START :
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_USRDEF :
		ret = state_user_defined(argc-2, &(argv[2]));
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_bridge.c
 *
 * Description  : ezbox ethernet bridge manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-05   0.1       Write it from scratch
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

static int bridge_user_defined(char *br_name, int argc, char **argv)
{
	if (argc < 1) {
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);

}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int rc_bridge(int argc, char **argv)
#endif
{
	char *br_name;
	int flag, ret;

	if (argc < 3) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "bridge")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	br_name = argv[1];

	flag = utils_get_rc_act_type(argv[2]);

	switch (flag) {
	case RC_ACT_BOOT :
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_STOP :
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_USRDEF :
		ret = bridge_user_defined(br_name, argc-3, &(argv[3]));
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

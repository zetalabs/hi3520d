/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_netbase.c
 *
 * Description  : ezbox env agent runs network base files service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
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
#include <net/if.h>

#include "ezcd.h"
#include "pop_func.h"

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_netbase(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "netbase")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
		/* manage network interfaces and configure some networking options */
		mkdir("/etc/network", 0755);
		mkdir("/etc/network/if-pre-up.d", 0755);
		mkdir("/etc/network/if-up.d", 0755);
		mkdir("/etc/network/if-down.d", 0755);
		mkdir("/etc/network/if-post-down.d", 0755);
		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_RESTART :
	case RC_ACT_START :
		pop_etc_network_interfaces(RC_ACT_START);
		pop_etc_hosts(RC_ACT_START);
		pop_etc_protocols(RC_ACT_START);
		pop_etc_mactab(RC_ACT_START);
		/* FIXME: do it after WAN interface is up */
#if 0
		pop_etc_resolv_conf(RC_ACT_START);
#endif
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}

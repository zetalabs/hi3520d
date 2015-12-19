/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_base_files.c
 *
 * Description  : ezbox env agent runs base files service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-02   0.1       Write it from scratch
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

#ifdef _EXEC_
int main(int argc, char **argv)
#else
int env_action_base_files(int argc, char **argv)
#endif
{
	int flag, ret;

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "base_files")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_START :
		/* set hostname */
		pop_etc_hostname(RC_ACT_START);
#if 0
		snprintf(cmdline, sizeof(cmdline), "%s /etc/hostname > /proc/sys/kernel/hostname", CMD_CAT);		
		utils_system(cmdline);
#endif

		/* generate /etc/profile */
		pop_etc_profile(RC_ACT_START);

		/* generate /etc/banner */
		pop_etc_banner(RC_ACT_START);

		/* generate /etc/mtab */
		pop_etc_mtab(RC_ACT_START);

		/* load LD_LIBRARY_PATH */
#if 0
		rc_ldconfig(RC_ACT_START);
#endif
		ret = EXIT_SUCCESS;
		break;

	default :
		ret = EXIT_FAILURE;
		break;
	}

	return (ret);
}

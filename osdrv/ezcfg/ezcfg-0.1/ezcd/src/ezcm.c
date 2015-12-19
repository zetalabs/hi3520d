/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcm.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * 2011-12-02   0.2       Modify it to use ezcfg API
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
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

static void ezcm_show_usage(void)
{
	printf("Usage: ezcm [-q] <command> [args]\n");
	printf("\n");
	printf("  [-q]--\n");
	printf("    run in quiet mode\n");
	printf("  <command>--\n");
	printf("    ezcfg control supported commands\n");
	printf("  [args]--\n");
	printf("    ezcfg control command arguments\n");
	printf("\n");
}

int ezcm_main(int argc, char **argv)
{
	int rc = 0;
	int i;
	bool quiet_mode = false;
	char buf[EZCFG_CTRL_MAX_MESSAGE_SIZE];
	int buf_len;

	if (argc < 2) {
		printf("need more arguments.\n");
		ezcm_show_usage();
		return -EZCFG_E_ARGUMENT ;
	}

#if 0
	rc = ezcfg_api_ctrl_set_config_file(EZCD_CONFIG_FILE_PATH);
	if (rc < 0) {
		printf("ERROR\n");
		return rc;
	}
#endif
	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		printf("ERROR\n");
		return -EZCFG_E_RESOURCE;
	}

	i = 1;
	if (strcmp(argv[i], "-q") == 0) {
		quiet_mode = true;
		i++;
	}
	buf_len = sizeof(buf);
	rc = ezcfg_api_ctrl_exec(&(argv[i]), buf, buf_len);
	if (quiet_mode == false) {
		if (rc < 0) {
			printf("ERROR\n");
		}
		else {
			printf("%s=%s\n", argv[i], buf);
		}
	}
        return rc;
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : upfw.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-01   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

static void upfw_show_usage(void)
{
	printf("Usage: upfw -f <filename> [-m <modelname>]\n");
	printf("\n");
	printf("  -f <filename>  set firmware file name\n");
	printf("  -m <modelname> check firmware model name\n");
	printf("  -h             show help info\n");
	printf("\n");
}

int upfw_main(int argc, char **argv)
{
	int c = 0;
	int rc = 0;
	char *fn = NULL;
	char *model = NULL;

	for (;;) {
		c = getopt(argc, argv, "f:m:h");
		if (c == EOF) break;
		switch (c) {
			case 'f' :
				fn = optarg;
				break;
			case 'm' :
				model = optarg;
				break;
			case 'h' :
			default:
				upfw_show_usage();
				return 0;
		}
	}

	if (fn == NULL) {
		upfw_show_usage();
		return 0;
	}

	printf("Start to upgrade firmware, please wait...\n");
	rc = ezcfg_api_firmware_upgrade(fn, model);
	if (rc < 0) {
		printf("ERROR\n");
	}
	else {
		printf("OK\n");
	}

        return rc;
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : nvram.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-22   0.1       Write it from scratch
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

static void nvram_show_usage(void)
{
	printf("Usage: nvram [commands]\n");
	printf("\n");
	printf("  [commands]--\n");
	printf("    get <name>          get nvram value by <name>\n");
	printf("    set <name> <value>  set nvram <name> with <value>\n");
	printf("    unset <name>        remove nvram <name> and its <value>\n");
	printf("    list                list all nvram\n");
	printf("    commit              save nvram to storage device\n");
	printf("    info                show nvram version and usage information\n");
	printf("\n");
}

int nvram_main(int argc, char **argv)
{
	int rc = 0;
	char *buf = NULL;
	int buf_len;

	if (argc < 2) {
		printf("need more arguments.\n");
		nvram_show_usage();
		return -EZCFG_E_ARGUMENT ;
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		printf("init ezcfg_api error.\n");
		return -EZCFG_E_RESOURCE ;
	}

	if (strcmp(argv[1], "get") == 0) {
		if (argc != 3) {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			buf_len = EZCFG_NVRAM_BUFFER_SIZE ;
			buf = (char *)malloc(buf_len);
			if (buf == NULL) {
				return -EZCFG_E_SPACE ;
			}
			rc = ezcfg_api_nvram_get(argv[2], buf, buf_len);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("%s=%s\n", argv[2], buf);
			}
			free(buf);
		}
		else {
			printf("the nvram name is empty.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "set") == 0) {
		if (argc != 4) {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			rc = ezcfg_api_nvram_set(argv[2], argv[3]);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("OK\n");
			}
		}
		else {
			printf("the nvram name is empty.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "unset") == 0) {
		if (argc != 3) {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
			goto exit;
		}

		if (strlen(argv[2]) > 0) {
			rc = ezcfg_api_nvram_unset(argv[2]);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("OK\n");
			}
		}
		else {
			printf("the nvram name is empty.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "list") == 0) {
		if (argc == 2) {
			buf_len = EZCFG_NVRAM_BUFFER_SIZE;
			buf = (char *)malloc(buf_len);
			if (buf == NULL) {
				return -EZCFG_E_SPACE ;
			}
			rc = ezcfg_api_nvram_list(buf, buf_len);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("%s", buf);
			}
			free(buf);
		}
		else {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "info") == 0) {
		if (argc == 2) {
			buf_len = EZCFG_NVRAM_BUFFER_SIZE;
			buf = (char *)malloc(buf_len);
			if (buf == NULL) {
				return -EZCFG_E_SPACE ;
			}
			rc = ezcfg_api_nvram_info(buf, buf_len);
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("%s", buf);
			}
			free(buf);
		}
		else {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else if (strcmp(argv[1], "commit") == 0) {
		if (argc == 2) {
			rc = ezcfg_api_nvram_commit();
			if (rc < 0) {
				printf("ERROR\n");
			}
			else {
				printf("OK\n");
			}
		}
		else {
			printf("number of arguments is incorrect.\n");
			nvram_show_usage();
			rc = -EZCFG_E_ARGUMENT ;
		}
	}
	else {
		printf("unknown nvram command: %s\n", argv[1]);
		nvram_show_usage();
		rc = -EZCFG_E_ARGUMENT ;
	}
exit:
        return rc;
}

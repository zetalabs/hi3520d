/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_sync_cfg_with_nvram.c
 *
 * Description  : ezcfg sync xxx.cfg with nvram keyword value function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-31   0.1       Write it from scratch
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

/*
 * prefix pattern format: [!][a-z|A-Z][a-z|A-Z|0-9|_|.]
 */
int utils_sync_cfg_with_nvram(char *path, char *pattern)
{
	FILE *file = NULL;
	char path2[64];
	FILE *file2 = NULL;
	char buf[FILE_LINE_BUFFER_SIZE];
	char buf2[FILE_LINE_BUFFER_SIZE];
	char *p = NULL;
	char *value = NULL;
	int ret = EXIT_FAILURE;
	char *prefix = pattern;
	int prefix_len = 0;
	int negative = 0;
	int len, rc;

	if (path == NULL)
		return ret;

	if (prefix != NULL) {
		if (*prefix == '!') {
			negative = 1;
			prefix++;
		}
		prefix_len = strlen(prefix);
	}

	file = fopen(path, "r");
	if (file == NULL) 
		return ret;

	snprintf(path2, sizeof(path2), "%s.new", path);
	file2 = fopen(path2, "w");
	if (file2 == NULL) {
		goto func_out;
	}

	while (utils_file_get_line(file, buf, sizeof(buf), "", LINE_TAIL_STRING) == true) {
		p = strchr(buf, '=');
		if (p != NULL) {
			*p = '\0';
			if (prefix_len > 0) {
				if (negative == 1) {
					/* skip syncing prefix-matched nvram */
					if (strncmp(buf, prefix, prefix_len) != 0) {
						value = p+1;
						len = sizeof(buf) - strlen(buf) - 1;
						rc = ezcfg_api_nvram_get(buf, buf2, sizeof(buf2));
						if (rc >= 0) {
							snprintf(value, len, "%s", buf2);
						}
					}
				}
				else {
					/* only sync prefix-matched nvram */
					if (strncmp(buf, prefix, prefix_len) == 0) {
						value = p+1;
						len = sizeof(buf) - strlen(buf) - 1;
						rc = ezcfg_api_nvram_get(buf, buf2, sizeof(buf2));
						if (rc >= 0) {
							snprintf(value, len, "%s", buf2);
						}
					}
				}
			}
			else {
				value = p+1;
				len = sizeof(buf) - strlen(buf) - 1;
				rc = ezcfg_api_nvram_get(buf, buf2, sizeof(buf2));
				if (rc >= 0) {
					snprintf(value, len, "%s", buf2);
				}
			}
			*p = '=';
		}
		fprintf(file2, "%s\n", buf);
	}

func_out:
	if (file != NULL)
		fclose(file);
	if (file2 != NULL) {
		fclose(file2);
		snprintf(buf, sizeof(buf), "%s -f %s %s", CMD_MV, path2, path);
		utils_system(buf);
	}
	return EXIT_SUCCESS;
}

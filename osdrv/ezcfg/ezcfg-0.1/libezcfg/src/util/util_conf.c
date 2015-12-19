/* ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : util/util_conf.c
 *
 * Description  : get string from config file
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-02-28   0.1       Write it from scrach
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
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) do { \
	char path[256]; \
	FILE *fp; \
	snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
	fp = fopen(path, "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

/*
 * Returns string from config file
 * It is the caller's duty to free the returned string.
 */
char *ezcfg_util_get_conf_string(const char *path,
	const char *section, const int idx, const char *keyword)
{
	FILE *file;
	char *p = NULL;
	char *v = NULL;
	char line[128];
	int i;
	size_t section_len;
	size_t keyword_len;

	if ((path == NULL) || (section == NULL) ||(keyword == NULL))
		return NULL;

	if (idx < 0)
		return NULL;

	section_len = strlen(section);
	if (section_len < 1)
		return NULL;

	keyword_len = strlen(keyword);
	if (keyword_len < 1)
		return NULL;

	if ((section_len > (sizeof(line) - 3)) ||
	    (keyword_len > (sizeof(line) - 2)))
		return NULL;

	/* get string from config file */
	file = fopen(path, "r");
	if (file == NULL) {
		return NULL;
	}

	i = -1; /* initialize index counter */

	/* find the index-th section part */
	while (fgets(line, sizeof(line), file) != NULL) {
		if ((line[0] == '[') &&
		    (strncmp(line+1, section, section_len) == 0) &&
		    (line[section_len+1] == ']')) {
			i++;
		}
		if (i == idx) {
			break;
		}
	}

	if (i == idx) {
		/* find target section */
		while (fgets(line, sizeof(line), file) != NULL) {
			if (line[0] == '[') {
				/* reach the next section */
				goto func_out;
			}
			if (strncmp(line, keyword, keyword_len) == 0) {
				p = line+keyword_len;
				if (*p == '=') {
					p++;
					ezcfg_util_remove_trailing_charlist(p, EZCFG_EZCFG_TRAILING_CHARLIST);
					v = strdup(p);
					goto func_out;
				}
			}
		}
	}

func_out:
	fclose(file);
	return v;
}

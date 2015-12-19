/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_freeradius2_dictionary.c
 *
 * Description  : ezbox /etc/freeradius2/dictionary file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-10-15   0.1       Write it from scratch
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

static int gen_additional_dictionary_entries(FILE *file)
{
	int rc;
	int i;
	int num;
	char name[64];
	char buf[256];

	/* dictionary */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, DICTIONARY_ENTRY_NUM), buf, sizeof(buf));
	if (rc > 0) {
		num = atoi(buf);
		for (i = 0; i < num; i++) {
			snprintf(name, sizeof(name), "%s.%d",
				NVRAM_SERVICE_OPTION(FREERADIUS2_SERVER, DICTIONARY_ENTRY), i);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\n", buf);
			}
		}
	}

	return EXIT_SUCCESS;
}

int pop_etc_freeradius2_dictionary(int flag)
{
        FILE *file = NULL;

	/* generate /etc/freeradius2/dictionary */
	file = fopen("/etc/freeradius2/dictionary", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	fprintf(file, "%s\n", "$INCLUDE /usr/share/freeradius2/dictionary");

	/* additional dirctionary entries */
	gen_additional_dictionary_entries(file);

	fclose(file);
	return (EXIT_SUCCESS);
}

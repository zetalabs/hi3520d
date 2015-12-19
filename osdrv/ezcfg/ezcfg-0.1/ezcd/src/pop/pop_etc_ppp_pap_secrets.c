/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ppp_pap_secrets.c
 *
 * Description  : ezbox /etc/ppp/pap-secrets file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-28   0.1       Write it from scratch
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

int pop_etc_ppp_pap_secrets(int flag)
{
        FILE *file = NULL;
	int rc;
	int i, entries = 0;
	char name[64];
	char buf[1024];

	/* generate /etc/ppp/pap-secrets */
	file = fopen("/etc/ppp/pap-secrets", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
		/* PAP secret entries number */
		rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(PPP, PAP_SECRET_ENTRY_NUM), buf, sizeof(buf));
		if (rc > 0) {
			entries = atoi(buf);
		}

		for (i = 0; i < entries; i++) {
			snprintf(name, sizeof(name), "%s%d", NVRAM_SERVICE_OPTION(PPP, PAP_SECRET_ENTRY), i);
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
			if (rc > 0) {
				fprintf(file, "%s\n", buf);
			}
		}

		break;
	}

	fclose(file);
	rc = chmod("/etc/ppp/pap-secrets", S_IRUSR | S_IWUSR);
	if (rc == 0)
		return (EXIT_SUCCESS);
	else
		return (EXIT_FAILURE);
}

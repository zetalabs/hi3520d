/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_login.c
 *
 * Description  : ezbox env agent runs login service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-03   0.1       Write it from scratch
 * 2011-10-16   0.2       Modify it to use rcso framework
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
int env_action_login(int argc, char **argv)
#endif
{
	FILE *fp;
	char type[32];
	char user[32];
	char passwd[64];
	char buf[64];
	pid_t pid;
	int i, auth_number;
	int ret, flag;
	char *chpasswd_argv[] = { CMD_CHPASSWD, NULL };

	if (argc < 2) {
		return (EXIT_FAILURE);
	}

	if (strcmp(argv[0], "login")) {
		return (EXIT_FAILURE);
	}

	if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
		return (EXIT_FAILURE);
	}

	flag = utils_get_rc_act_type(argv[1]);

	switch (flag) {
	case RC_ACT_BOOT :
		/* generate /etc/passwd */
		pop_etc_passwd(RC_ACT_BOOT);

		/* generate /etc/group */
		pop_etc_group(RC_ACT_BOOT);

		ret = EXIT_SUCCESS;
		break;

	case RC_ACT_START :
		/* generate /etc/passwd */
		pop_etc_passwd(RC_ACT_START);

		/* generate /etc/group */
		pop_etc_group(RC_ACT_START);

		/* fall down to change passwd */

	case RC_ACT_RESTART :
		ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, COMMON_AUTH_NUMBER), buf, sizeof(buf));
		if (ret < 0) {
			ret = EXIT_FAILURE;
			break;
		}
		auth_number = atoi(buf);
		if (auth_number < 1) {
			ret = EXIT_FAILURE;
			break;
		}

		/* get user name and password */
		ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_USER), user, sizeof(user));
		if (ret < 0) {
			ret = EXIT_FAILURE;
			break;
		}

		ret = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(EZCFG, AUTH_0_SECRET), passwd, sizeof(passwd));
		if (ret < 0) {
			ret = EXIT_FAILURE;
			break;
		}

		/* set root password */
		pid = getpid();
		snprintf(buf, sizeof(buf), "/tmp/rc_login.%d", pid);
		fp = fopen(buf, "w");
		if (fp == NULL) {
			break;
		}
		for (i = 0; i < auth_number; i++) {
			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_TYPE);
			ret = ezcfg_api_nvram_get(buf, type, sizeof(type));
			if (ret < 0) {
				continue;
			}
			if (strcmp(type, EZCFG_AUTH_TYPE_HTTP_BASIC_STRING) != 0) {
				continue;
			}

			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_USER);
			ret = ezcfg_api_nvram_get(buf, user, sizeof(user));
			if (ret < 0) {
				continue;
			}

			snprintf(buf, sizeof(buf), "%s%s.%d.%s",
				EZCFG_EZCFG_NVRAM_PREFIX,
				EZCFG_EZCFG_SECTION_AUTH,
				i, EZCFG_EZCFG_KEYWORD_SECRET);
			ret = ezcfg_api_nvram_get(buf, passwd, sizeof(passwd));
			if (ret < 0) {
				continue;
			}
			fprintf(fp, "%s:%s\n", user, passwd);
		}
		fclose(fp);
#if 0
		snprintf(buf, sizeof(buf), "%s /tmp/rc_login.%d | %s", CMD_CAT, pid, CMD_CHPASSWD);
		utils_system(buf);
		snprintf(buf, sizeof(buf), "%s -rf /tmp/rc_login.%d", CMD_RM, pid);
		utils_system(buf);
#else
		snprintf(buf, sizeof(buf), "/tmp/rc_login.%d", pid);
		utils_execute(chpasswd_argv, buf, NULL, 0, NULL);
		unlink(buf);
#endif

		ret = EXIT_SUCCESS;
		break;

	default:
		ret = EXIT_FAILURE;
		break;
	}
	return (ret);
}

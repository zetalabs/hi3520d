/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_check_ezcd.c
 *
 * Description  : ezbox check ezcfg daemon service status
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-09   0.1       Write it from scratch
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

#if 0
#define DBG(format, args...) do {\
	FILE *fp = fopen("/dev/kmsg", "a"); \
	if (fp) { \
		fprintf(fp, format, ## args); \
		fclose(fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

bool utils_ezcd_is_alive(void)
{
	return utils_has_process_by_name("ezcd");
}

bool utils_ezcd_is_ready(void)
{
	char buf[64];
	int rc;

	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(SYS, SERIAL_NUMBER), buf, sizeof(buf));
	if (rc < 0) {
		DBG("pid=[%d] %s(%d) rc=[%d]\n", getpid(), __func__, __LINE__, rc);
		return false;
	}
	else {
		return true;
	}
}

bool utils_ezcd_wait_up(int s)
{
	if (s < 1) {
		while (utils_ezcd_is_ready() == false) {
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
		}
		return true;
	}
	else {
		while(s > 0) {
			if (utils_ezcd_is_ready() == true) {
				return true;
			}
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
			s--;
		}
		return false;
	}
}

bool utils_ezcd_wait_down(int s)
{
	if (s < 1) {
		while (utils_ezcd_is_alive() == true) {
			DBG("<6>pid=[%d] %s(%d) sleep 1 seconds\n", getpid(), __func__, __LINE__);
			sleep(1);
		}
		return true;
	}
	else {
		while(s > 0) {
			if (utils_ezcd_is_alive() == false) {
				return true;
			}
			DBG("%s(%d) sleep 1 seconds\n", __func__, __LINE__);
			sleep(1);
			s--;
		}
		return false;
	}
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : rc_wan_services.c
 *
 * Description  : ezbox run WAN services
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-10   0.1       Write it from scratch
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
#include <net/if.h>

#include "ezcd.h"
#include "rc_func.h"

int rc_wan_services(int flag)
{
	int ret = EXIT_FAILURE;

	switch (flag) {
	case RC_BOOT :
	case RC_START :
		/* start ezcfg httpd service */
		rc_wan_ezcfg_httpd(RC_START);

#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
		/* start telnet service */
		rc_wan_telnetd(RC_START);
#endif

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
		/* start dnsmasq service */
		rc_wan_dnsmasq(RC_START);
#endif

#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
		/* start iptables-provided service */
		rc_wan_iptables(RC_START);
#endif

		ret = EXIT_SUCCESS;
		break;

	case RC_STOP :
#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
		/* stop iptables-provided service */
		rc_wan_iptables(RC_STOP);
#endif

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
		/* stop dnsmasq service */
		rc_wan_dnsmasq(RC_STOP);
#endif

#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
		/* stop telnet service */
		rc_wan_telnetd(RC_STOP);
#endif

		/* stop ezcfg httpd service */
		rc_wan_ezcfg_httpd(RC_STOP);

		ret = EXIT_SUCCESS;
		break;
	}

	return ret;
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ppp_rp_pppoe_options.c
 *
 * Description  : ezbox /etc/ppp/rp-pppoe-options file generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-09-13   0.1       Write it from scratch
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

static int gen_ppp_general_options(FILE *file)
{
	int rc;
	char buf[256];

	/* general options */
	fprintf(file, "# %s\n", "general options");

	/* debug */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_DEBUG), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_DEBUG, rc);
		}
	}

	/* kdebug */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_KDEBUG), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_KDEBUG, rc);
		}
	}

	/* nodetach */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NODETACH), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NODETACH);
		}
	}

	/* updetach */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_UPDETACH), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_UPDETACH);
		}
	}

	/* holdoff */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_HOLDOFF), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_HOLDOFF, rc);
		}
	}

	/* idle */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IDLE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_IDLE, rc);
		}
	}

	/* maxconnect */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MAXCONNECT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_MAXCONNECT, rc);
		}
	}

	/* persist */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PERSIST), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_PERSIST);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOPERSIST);
		}
	}

	/* demand */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_DEMAND), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_DEMAND);
		}
	}

	/* logfile */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LOGFILE), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_LOGFILE, buf);
	}

	/* linkname */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LINKNAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_LINKNAME, buf);
	}

	/* ifname */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IFNAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IFNAME, buf);
	}

	/* maxfail */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MAXFAIL), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_MAXFAIL, rc);
		}
	}

	/* ktune */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_KTUNE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_KTUNE);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOKTUNE);
		}
	}

	/* connect-delay */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_CONNECT_DELAY), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_CONNECT_DELAY, rc);
		}
	}

	/* unit */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_UNIT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc >= 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_UNIT, rc);
		}
	}

	/* dump */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_DUMP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_DUMP);
		}
	}

	/* dryrun */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_DRYRUN), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_DRYRUN);
		}
	}

	/* child-timeout */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_CHILD_TIMEOUT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_CHILD_TIMEOUT, rc);
		}
	}

	/* ip-up-script */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IP_UP_SCRIPT), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IP_UP_SCRIPT, buf);
	}

	/* ip-down-script */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IP_DOWN_SCRIPT), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IP_DOWN_SCRIPT, buf);
	}

	/* ipv6-up-script */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPV6_UP_SCRIPT), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IPV6_UP_SCRIPT, buf);
	}

	/* ipv6-down-script */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPV6_DOWN_SCRIPT), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IPV6_DOWN_SCRIPT, buf);
	}

	/* plugin */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PLUGIN), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_PLUGIN, buf);
	}

	return EXIT_SUCCESS;
}

static int gen_ppp_auth_options(FILE *file)
{
	int rc;
	char buf[256];

	/* auth options */
	fprintf(file, "# %s\n", "auth options");

	/* auth */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_AUTH), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_AUTH);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOAUTH);
		}
	}

	/* require-pap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REQUIRE_PAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REQUIRE_PAP);
		}
	}

	/* require-chap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REQUIRE_CHAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REQUIRE_CHAP);
		}
	}

	/* require-mschap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REQUIRE_MSCHAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REQUIRE_MSCHAP);
		}
	}

	/* require-mschap-v2 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REQUIRE_MSCHAP_V2), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REQUIRE_MSCHAP_V2);
		}
	}

	/* refuse-pap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REFUSE_PAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REFUSE_PAP);
		}
	}

	/* refuse-chap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REFUSE_CHAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REFUSE_CHAP);
		}
	}

	/* refuse-mschap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REFUSE_MSCHAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REFUSE_MSCHAP);
		}
	}

	/* refuse-mschap-v2 */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REFUSE_MSCHAP_V2), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REFUSE_MSCHAP_V2);
		}
	}

	/* require-eap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REQUIRE_EAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REQUIRE_EAP);
		}
	}

	/* refuse-eap */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REFUSE_EAP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REFUSE_EAP);
		}
	}

	/* name */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_NAME, buf);
	}

	/* user */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_USER), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_USER, buf);
	}

	/* password */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PASSWORD), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_PASSWORD, buf);
	}

	/* usehostname */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_USEHOSTNAME), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_USEHOSTNAME);
		}
	}

	/* remotename */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REMOTENAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_REMOTENAME, buf);
	}

	/* login */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LOGIN), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_LOGIN);
		}
	}

	/* enable-session */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_ENABLE_SESSION), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_ENABLE_SESSION);
		}
	}

	/* papcrypt */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PAPCRYPT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_PAPCRYPT);
		}
	}

	/* privgroup */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PRIVGROUP), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_PRIVGROUP, buf);
	}

	/* allow-ip */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_ALLOW_IP), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_ALLOW_IP, buf);
	}

	/* remotenumber */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REMOTENUMBER), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_REMOTENUMBER, buf);
	}

	/* allow-number */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_ALLOW_NUMBER), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_ALLOW_NUMBER, buf);
	}

	return EXIT_SUCCESS;
}

static int gen_ppp_lcp_options(FILE *file)
{
	int rc;
	char buf[256];

	/* lcp options */
	fprintf(file, "# %s\n", "lcp options");

	/* -all */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_NOOPT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_MINUS_ALL);
			/* Don't request/allow any LCP options */
			return EXIT_SUCCESS;
		}
	}

	/* noaccomp */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOACCOMP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOACCOMP);
		}
	}

	/* nomagic */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOMAGIC), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOMAGIC);
		}
	}

	/* mru */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MRU), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_MRU, rc);
		}
	}

	/* mtu */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MTU), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_MTU, rc);
		}
	}

	/* nopcomp */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOPCOMP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOPCOMP);
		}
	}

	/* passive */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PASSIVE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_PASSIVE);
		}
	}

	/* silent */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_SILENT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_SILENT);
		}
	}

	/* lcp-echo-failure */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_ECHO_FAILURE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_ECHO_FAILURE, rc);
		}
	}

	/* lcp-echo-interval */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_ECHO_INTERVAL), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_ECHO_INTERVAL, rc);
		}
	}

	/* lcp-restart */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_RESTART), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_RESTART, rc);
		}
	}

	/* lcp-max-terminate */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_MAX_TERMINATE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_MAX_TERMINATE, rc);
		}
	}

	/* lcp-max-configure */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_MAX_CONFIGURE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_MAX_CONFIGURE, rc);
		}
	}

	/* lcp-max-failure */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LCP_MAX_FAILURE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_LCP_MAX_FAILURE, rc);
		}
	}

	/* receive-all */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_RECEIVE_ALL), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_RECEIVE_ALL);
		}
	}

	/* noendpoint */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOENDPOINT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOENDPOINT);
		}
	}

	return EXIT_SUCCESS;
}

static int gen_ppp_ipcp_options(FILE *file)
{
	int rc;
	char buf[256];

	/* ipcp options */
	fprintf(file, "# %s\n", "ipcp options");

	/* noip */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOIP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOIP);
			/* no other IPCP options */
			return EXIT_SUCCESS;
		}
	}

	/* novj */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOVJ), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOVJ);
		}
	}

	/* novjccomp */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOVJCCOMP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOVJCCOMP);
		}
	}

	/* vj-max-slots */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_VJ_MAX_SLOTS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if ((rc >= 2) && (rc <= 16)) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_VJ_MAX_SLOTS, rc);
		}
	}

	/* ipcp-accept-local */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_ACCEPT_LOCAL), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_IPCP_ACCEPT_LOCAL);
		}
	}

	/* ipcp-accept-remote */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_ACCEPT_REMOTE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_IPCP_ACCEPT_REMOTE);
		}
	}

	/* ipparam */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPPARAM), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IPPARAM, buf);
	}

	/* noipdefault */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOIPDEFAULT), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOIPDEFAULT);
		}
	}

	/* ms-dns */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MS_DNS), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_MS_DNS, buf);
	}

	/* ms-wins */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_MS_WINS), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_MS_WINS, buf);
	}

	/* ipcp-restart */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_RESTART), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_IPCP_RESTART, rc);
		}
	}

	/* ipcp-max-terminate */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_MAX_TERMINATE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_TERMINATE, rc);
		}
	}

	/* ipcp-max-configure */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_MAX_CONFIGURE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_CONFIGURE, rc);
		}
	}

	/* ipcp-max-failure */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_MAX_FAILURE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_IPCP_MAX_FAILURE, rc);
		}
	}

	/* defaultroute */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_DEFAULTROUTE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_DEFAULTROUTE);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NODEFAULTROUTE);
		}
	}

	/* replacedefaultroute */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_REPLACEDEFAULTROUTE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_REPLACEDEFAULTROUTE);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOREPLACEDEFAULTROUTE);
		}
	}

	/* proxyarp */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_PROXYARP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_PROXYARP);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOPROXYARP);
		}
	}

	/* usepeerdns */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_USEPEERDNS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_USEPEERDNS);
		}
	}

	/* netmask */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NETMASK), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_NETMASK, buf);
	}

	/* ipcp-no-addresses */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_NO_ADDRESSES), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_IPCP_NO_ADDRESSES);
		}
	}

	/* ipcp-no-address */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IPCP_NO_ADDRESS), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_IPCP_NO_ADDRESS);
		}
	}

	/* noremoteip */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOREMOTEIP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOREMOTEIP);
		}
	}

	/* nosendip */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_NOSENDIP), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOSENDIP);
		}
	}

	/* IP addresses */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_IP_ADDRESSES), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_IP_ADDRESSES, buf);
	}

	return EXIT_SUCCESS;
}

static int gen_ppp_tty_options(FILE *file)
{
	int rc;
	char buf[256];

	/* tty options */
	fprintf(file, "# %s\n", "tty options");

	/* lock */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPP_LOCK), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_LOCK);
		}
		else {
			fprintf(file, "%s\n", EZCFG_PPP_OPT_KEYWORD_NOLOCK);
		}
	}

	return EXIT_SUCCESS;
}

static int gen_ppp_plugin_rp_pppoe_options(FILE *file)
{
	int rc;
	char buf[256];

	/* ppp plugin rp-pppoe options */
	fprintf(file, "# %s\n", "ppp plugin rp-pppoe options");

	/* device name */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, DEVICE_NAME), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s\n", buf);
	}

	/* rp_pppoe_service */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, RP_PPPOE_SERVICE), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_SERVICE, buf);
	}

	/* rp_pppoe_ac */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, RP_PPPOE_AC), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_AC, buf);
	}

	/* rp_pppoe_sess */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, RP_PPPOE_SESS), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_SESS, buf);
	}

	/* rp_pppoe_verbose */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, RP_PPPOE_VERBOSE), buf, sizeof(buf));
	if (rc > 0) {
		rc = atoi(buf);
		if (rc > 0) {
			fprintf(file, "%s %d\n", EZCFG_PPP_OPT_KEYWORD_RP_PPPOE_VERBOSE, rc);
		}
	}

	/* pppoe-mac */
	rc = ezcfg_api_nvram_get(NVRAM_SERVICE_OPTION(RP_PPPOE, PPPOE_MAC), buf, sizeof(buf));
	if (rc > 0) {
		fprintf(file, "%s %s\n", EZCFG_PPP_OPT_KEYWORD_PPPOE_MAC, buf);
	}

	return EXIT_SUCCESS;
}

int pop_etc_ppp_rp_pppoe_options(int flag)
{
        FILE *file = NULL;

	/* generate /etc/ppp/rp-pppoe-options */
	file = fopen("/etc/ppp/rp-pppoe-options", "w");
	if (file == NULL)
		return (EXIT_FAILURE);

	/* general options */
	gen_ppp_general_options(file);

	/* auth options */
	gen_ppp_auth_options(file);

	/* ppp/lcp options */
	gen_ppp_lcp_options(file);

	/* ppp/ipcp options */
	gen_ppp_ipcp_options(file);

	/* ppp/tty options */
	gen_ppp_tty_options(file);

	/* extra options */
	gen_ppp_plugin_rp_pppoe_options(file);

	fclose(file);
	return (EXIT_SUCCESS);
}

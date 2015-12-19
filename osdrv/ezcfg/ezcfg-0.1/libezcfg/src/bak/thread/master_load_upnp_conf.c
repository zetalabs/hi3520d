/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : thread/master_load_upnp_conf.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-29   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>

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

/* setup UPnP root devices and control points */
void ezcfg_master_load_upnp_conf(struct ezcfg_master *master)
{
	struct ezcfg *ezcfg;
	struct ezcfg_upnp *up = NULL;
	struct ezcfg_upnp **pup = NULL;
	char *p, *q, *r;
	int role, type;
	char path[256];
	char ifname[IFNAMSIZ];
	int i;
	int upnp_number = -1;

	if (master == NULL)
		return ;

	ezcfg = ezcfg_master_get_ezcfg(master);

	/* first get the upnp number */
	p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_COMMON, 0, EZCFG_EZCFG_KEYWORD_UPNP_NUMBER);
	if (p != NULL) {
		upnp_number = atoi(p);
		free(p);
	}

	for (i = 0; i < upnp_number; i++) {
		/* initialize */
		up = ezcfg_upnp_new(ezcfg);

		if (up == NULL) {
			continue;
		}

		/* UPnP device role */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_UPNP, i, EZCFG_EZCFG_KEYWORD_ROLE);
		if (p == NULL) {
			ezcfg_upnp_delete(up);
			continue;
		}

		role = ezcfg_util_upnp_role(p);
		free(p);

		if (role == EZCFG_UPNP_ROLE_UNKNOWN) {
			ezcfg_upnp_delete(up);
			continue;
		}
		ezcfg_upnp_set_role(up, role);

		/* UPnP device type */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_UPNP, i, EZCFG_EZCFG_KEYWORD_DEVICE_TYPE);
		if (p == NULL) {
			ezcfg_upnp_delete(up);
			continue;
		}

		type = ezcfg_util_upnp_device_type(p);
		free(p);

		if (type == EZCFG_UPNP_DEV_TYPE_UNKNOWN) {
			ezcfg_upnp_delete(up);
			continue;
		}
		ezcfg_upnp_set_device_type(up, type);

		/* UPnP description */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_UPNP, i, EZCFG_EZCFG_KEYWORD_DESCRIPTION_PATH);
		if (p == NULL) {
			ezcfg_upnp_delete(up);
			continue;
		}
		snprintf(path, sizeof(path), "%s", p);
		free(p);

		/* UPnP interface */
		p = ezcfg_util_get_conf_string(ezcfg_common_get_config_file(ezcfg), EZCFG_EZCFG_SECTION_UPNP, i, EZCFG_EZCFG_KEYWORD_INTERFACE);
		if (p == NULL) {
			ezcfg_upnp_delete(up);
			continue;
		}
		q = p;
		while(q != NULL) {
			char *p_life_time;
			int life_time;

			r = strchr(q, ',');
			if (r != NULL) {
				*r = '\0';
				r++;
			}

			life_time = EZCFG_UPNP_SSDP_ADVERTISE_EXPIRE_TIME;

			p_life_time = strchr(q, '@');
			if (p_life_time != NULL) {
				*p_life_time = '\0';
				p_life_time++;
				life_time = atoi(p_life_time);
			}

			snprintf(ifname, sizeof(ifname), "%s", q);
			ezcfg_upnp_if_list_insert(up, ifname, life_time);
			q = r;
		}
		free(p);

		/* Add upnp info structure */
		if (ezcfg_upnp_parse_description(up, path) == false) {
			ezcfg_upnp_delete(up);
			continue;
		}

#if 0
		/* check if upnp is already set */
		pup = ezcfg_master_get_p_upnp(master);
		if (ezcfg_upnp_list_in(pup, up) == true) {
			info(ezcfg, "upnp entry already set\n");
			ezcfg_upnp_delete(up);
			continue;
		}
#endif

		/* add new upnp */
		pup = ezcfg_master_get_p_upnp(master);
		if (ezcfg_upnp_list_insert(pup, up) == true) {
			info(ezcfg, "insert upnp entry successfully\n");
			/* set up to NULL to avoid delete it */
			up = NULL;
		}
		else {
			err(ezcfg, "insert upnp entry failed: %m\n");
			ezcfg_upnp_delete(up);
		}
	}
}

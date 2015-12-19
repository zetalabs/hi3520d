/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ctrl/ctrl_upnp.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-12-06   0.1       Write it from scratch
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
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>
#include <sys/sysinfo.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 0
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

int ezcfg_ctrl_handle_upnp_message(char **argv, char *output, int len, void *rte)
{
	struct ezcfg *ezcfg;
	struct ezcfg_worker *worker;
	struct ezcfg_master *master;
	struct ezcfg_upnp *upnp;
	struct ezcfg_upnp_ssdp *ssdp;
	int rc;

	if (argv == NULL || argv[0] == NULL) {
		return -1;
	}

	if (strcmp(argv[0], "upnp") != 0) {
		return -1;
	}

	if (argv[1] == NULL) {
		return -1;
	}

	worker = rte;
	ezcfg = ezcfg_worker_get_ezcfg(worker);
	master = ezcfg_worker_get_master(worker);

	if (strcmp(argv[1], "monitor") == 0) {
		FILE *fp;
		struct sysinfo si;
		char buf[256];

		if (argv[2] == NULL) {
			return -1;
		}

		if (strcmp(argv[2], "start") == 0) {
			if (sysinfo(&si) < 0) {
				return -1;
			}

			if (ezcfg_util_upnp_monitor_lock_task_file(ezcfg_common_get_sem_ezcfg_path(ezcfg)) == false) {
				return -1;
			}
			rc = ezcfg_util_upnp_monitor_get_task_file(buf, sizeof(buf));
			if (rc < 0) {
				return (rc);
			}
			rc = -1;
			fp = fopen(buf, "a");
			if (fp != NULL) {
				/* add notify alive task */
				fprintf(fp, "%s:%s:%ld:%d\n",
					"add",
					"ezcm -q upnp ssdp notify_alive",
					si.uptime, 30);
				fclose(fp);
				rc = 0;
			}
			ezcfg_util_upnp_monitor_unlock_task_file(ezcfg_common_get_sem_ezcfg_path(ezcfg));
			return (rc);
		}
		else if (strcmp(argv[2], "stop") == 0) {
			if (sysinfo(&si) < 0) {
				return -1;
			}

			if (ezcfg_util_upnp_monitor_lock_task_file(ezcfg_common_get_sem_ezcfg_path(ezcfg)) == false) {
				return -1;
			}
			rc = ezcfg_util_upnp_monitor_get_task_file(buf, sizeof(buf));
			if (rc < 0) {
				return (rc);
			}
			rc = -1;
			fp = fopen(buf, "a");
			if (fp != NULL) {
				/* delete notify alive task */
				fprintf(fp, "%s:%s:%ld:%d\n",
					"del",
					"ezcm -q upnp ssdp notify_alive",
					si.uptime, 30);
				/* add notify byebye task */
				fprintf(fp, "%s:%s:%ld:%d\n",
					"add",
					"ezcm -q upnp ssdp notify_byebye",
					0L, 0);
				fclose(fp);
				rc = 0;
			}
			ezcfg_util_upnp_monitor_unlock_task_file(ezcfg_common_get_sem_ezcfg_path(ezcfg));
			return (rc);
		}
		else if (strcmp(argv[2], "task_file") == 0) {
			if (argv[3] == NULL) {
				return -1;
			}
			return ezcfg_util_upnp_monitor_set_task_file(argv[3]);
		}
	}
	else if (strcmp(argv[1], "ssdp") == 0) {
		if (argv[2] == NULL) {
			return -1;
		}

		ssdp = ezcfg_upnp_ssdp_new(ezcfg);
		if (ssdp == NULL) {
			return -1;
		}

		if (strcmp(argv[2], "notify_alive") == 0) {
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					ezcfg_upnp_ssdp_notify_alive(ssdp);
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}
		else if (strcmp(argv[2], "notify_byebye") == 0) {
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					ezcfg_upnp_ssdp_notify_byebye(ssdp);
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}
		else if (strcmp(argv[2], "msearch_request") == 0) {
			char *ST = argv[3];
			if ((ST == NULL) || (*ST == '\0')) {
				ST = "ssdp:all";
			}
			if (ezcfg_master_upnp_mutex_lock(master) == 0) {
				upnp = ezcfg_master_get_upnp(master);
				if (upnp != NULL) {
					ezcfg_upnp_ssdp_set_upnp(ssdp, upnp);
					if (ezcfg_upnp_ssdp_set_priv_data(ssdp, ST) == true) {
						ezcfg_upnp_ssdp_msearch_request(ssdp);
					}
				}
				ezcfg_master_upnp_mutex_unlock(master);
				ezcfg_upnp_ssdp_delete(ssdp);
				return 0;
			}
		}

		ezcfg_upnp_ssdp_delete(ssdp);
	}
	return -1;
}

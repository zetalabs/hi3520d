/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : ezcm.c
 *
 * Description  : ezbox config interface
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
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

static bool debug = false;

static void log_fn(struct ezcfg *ezcfg, int priority,
                   const char *file, int line, const char *fn,
                   const char *format, va_list args)
{
	if (debug) {
		char buf[1024];
		struct timeval tv;
		struct timezone tz;

		vsnprintf(buf, sizeof(buf), format, args);
		gettimeofday(&tv, &tz);
		fprintf(stderr, "%llu.%06u [%u] %s(%d): %s",
		        (unsigned long long) tv.tv_sec, (unsigned int) tv.tv_usec,
		        (int) getpid(), fn, line, buf);
	} else {
		vsyslog(priority, format, args);
        }
}

static void ezcm_show_usage(void)
{
	printf("Usage: ezcm -s service_id [-a invoke_args]\n");
	printf("\n");
	printf("  -s\ttarget service id\n");
	printf("  -a\tinvoke arguments\n");
	printf("  -D\tdebug mode\n");
	printf("  -h\thelp\n");
	printf("\n");
}

int ezcm_main(int argc, char **argv)
{
	int c = 0;
	int rc = 0;
	char buf[256];
	char *msg = NULL;
	int msg_len;
	char *p;
	struct ezcfg *ezcfg = NULL;
	struct ezcfg_uuid *uuid = NULL;
	struct ezcfg_igrs *igrs = NULL;
	struct ezcfg_ctrl *ezctrl = NULL;
	unsigned int service_id;
	char *invoke_args;
	time_t t;

	if (argc < 3) {
		ezcm_show_usage();
		return 0;
	}

	memset(buf, 0, sizeof(buf));
	service_id = 0;
	invoke_args = NULL;

	for (;;) {
		c = getopt(argc, argv, "Dha:m:s:");
		if (c == EOF) break;
		switch (c) {
			case 's':
				service_id = (unsigned int)atoi(optarg);
				break;
			case 'a':
				invoke_args = optarg;
				break;
			case 'D':
				debug = true;
				break;
			case 'h':
			default:
				ezcm_show_usage();
				return 0;
		}
	}

	if (service_id == 0) {
		printf("service id must larger than 0.\n");
		return 0;
	}

	ezcfg = ezcfg_new(EZCD_CONFIG_FILE_PATH);
	if (ezcfg == NULL) {
		printf("error : %s\n", "ezcfg_new");
		rc = 1;
		goto exit;
	}

	ezcfg_log_init("ezcm");
	ezcfg_common_set_log_fn(ezcfg, log_fn);
	info(ezcfg, "version %s\n", VERSION);

	igrs = ezcfg_igrs_new(ezcfg);
	if (igrs == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm igrs builder");
		rc = 2;
		goto exit;
	}

	ezcfg_igrs_set_version_major(igrs, 1);
	ezcfg_igrs_set_version_minor(igrs, 0);

	uuid = ezcfg_uuid_new(ezcfg, 1);
	if (uuid == NULL) {
		err(ezcfg, "%s\n", "Cannot init uuid");
		goto exit;
	}
	if (ezcfg_uuid_generate(uuid) == true) {
		char uuid_str[EZCFG_UUID_STRING_LEN+1];
		ezcfg_uuid_export_str(uuid, uuid_str, sizeof(uuid_str));
		ezcfg_igrs_set_source_device_id(igrs, uuid_str);
	} else {
		ezcfg_igrs_set_source_device_id(igrs, EZCFG_UUID_NIL_STRING);
	}
	if (ezcfg_uuid_generate(uuid) == true) {
		char uuid_str[EZCFG_UUID_STRING_LEN+1];
		ezcfg_uuid_export_str(uuid, uuid_str, sizeof(uuid_str));
		ezcfg_igrs_set_target_device_id(igrs, uuid_str);
	} else {
		ezcfg_igrs_set_target_device_id(igrs, EZCFG_UUID_NIL_STRING);
	}

	srand((unsigned)time(&t));
	ezcfg_igrs_set_source_client_id(igrs, rand());
	ezcfg_igrs_set_target_service_id(igrs, service_id);
	ezcfg_igrs_set_sequence_id(igrs, rand());
	ezcfg_igrs_set_source_user_id(igrs, "igrs-tester");
	ezcfg_igrs_set_service_security_id(igrs, NULL);
	if (invoke_args != NULL) {
		ezcfg_igrs_set_invoke_args(igrs, invoke_args);
	}
	ezcfg_igrs_set_message_type(igrs, EZCFG_IGRS_MSG_INVOKE_SERVICE_REQUEST);
	ezcfg_igrs_build_message(igrs);
	msg_len = ezcfg_igrs_get_message_length(igrs);
	if (msg_len < 0) {
		err(ezcfg, "ezcfg_igrs_get_message_length error.\n");
		rc = 1;
		goto exit;
	}
	msg_len++; /* one more for '\0' */
	msg = (char *)malloc(msg_len);
	if (msg == NULL) {
		err(ezcfg, "malloc msg error.\n");
		rc = 1;
		goto exit;
	}
	memset(msg, 0, msg_len);
	msg_len = ezcfg_igrs_write_message(igrs, msg, msg_len);

	snprintf(buf, sizeof(buf), "%s-%d", EZCFG_SOCK_CTRL_PATH, getpid());

	ezctrl = ezcfg_ctrl_new_from_socket(ezcfg, AF_LOCAL, EZCFG_PROTO_IGRS, buf, EZCFG_SOCK_CTRL_PATH);

	if (ezctrl == NULL) {
		err(ezcfg, "%s\n", "Cannot initialize ezcm controller");
		rc = 2;
		goto exit;
	}

	if (ezcfg_ctrl_connect(ezctrl) < 0) {
		err(ezcfg, "controller connect fail: %m\n");
		rc = 3;
		goto exit;
	}

	if (ezcfg_ctrl_write(ezctrl, msg, msg_len, 0) < 0) {
		err(ezcfg, "controller write: %m\n");
		rc = 4;
		goto exit;
	}

	p = realloc(msg, EZCFG_IGRS_HTTP_CHUNK_SIZE);
	if (p != NULL) {
		msg = p;
		msg_len = EZCFG_IGRS_HTTP_CHUNK_SIZE;
	}

	memset(msg, 0, msg_len);

	if (ezcfg_ctrl_read(ezctrl, msg, msg_len, 0) < 0) {
		err(ezcfg, "controller write: %m\n");
		rc = 5;
		goto exit;
	}
exit:
	if (msg != NULL)
		free(msg);

	if (uuid != NULL)
		ezcfg_uuid_delete(uuid);

	if (igrs != NULL)
		ezcfg_igrs_delete(igrs);

	if (ezctrl != NULL)
		ezcfg_ctrl_delete(ezctrl);

	if (ezcfg != NULL)
		ezcfg_delete(ezcfg);

	return rc;
}

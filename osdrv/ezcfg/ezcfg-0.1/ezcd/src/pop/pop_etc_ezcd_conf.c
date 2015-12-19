/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : pop_etc_ezcd_conf.c
 *
 * Description  : ezbox /etc/ezcd.conf generating program
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-24   0.1       Write it from scratch
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

#if 0
#define DBG(format, args...) do {\
	FILE *dbg_fp = fopen("/dev/kmsg", "a"); \
	if (dbg_fp) { \
		fprintf(dbg_fp, format, ## args); \
		fclose(dbg_fp); \
	} \
} while(0)
#else
#define DBG(format, args...)
#endif

static int generate_path_conf(FILE *file, int flag)
{
	char name[64];
	char buf[256];
	int rc;

	DBG("%s(%d) flag=[%d] RC_ACT_BOOT=[%d]\n", __func__, __LINE__, flag, RC_ACT_BOOT);

	/* rules_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_RULES_PATH);
	DBG("%s(%d) name=[%s]\n", __func__, __LINE__, name);
	if (flag == RC_ACT_BOOT) {
		DBG("%s(%d) RC_ACT_BOOT\n", __func__, __LINE__);
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		DBG("%s(%d) not RC_ACT_BOOT\n", __func__, __LINE__);
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_RULES_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_RULES_PATH, buf);
	}

	/* root_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_ROOT_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_ROOT_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ROOT_PATH, buf);
	}

	/* sem_ezcfg_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SEM_EZCFG_PATH, buf);
	}

	/* shm_ezcfg_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_PATH, buf);
	}

	/* shm_ezcfg_size */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCFG_SIZE);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_SIZE, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_SIZE, buf);
	}

	/* shm_ezcfg_nvram_queue_length */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCFG_NVRAM_QUEUE_LENGTH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_NVRAM_QUEUE_LENGTH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_NVRAM_QUEUE_LENGTH, buf);
	}

	/* shm_ezcfg_rc_queue_length */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCFG_RC_QUEUE_LENGTH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_RC_QUEUE_LENGTH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCFG_RC_QUEUE_LENGTH, buf);
	}

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
	/* shm_ezctp_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCTP_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_PATH, buf);
	}

	/* shm_ezctp_size */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCTP_SIZE);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_SIZE, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_SIZE, buf);
	}

	/* shm_ezctp_cq_unit_size */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_UNIT_SIZE);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_UNIT_SIZE, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_UNIT_SIZE, buf);
	}

	/* shm_ezctp_cq_length */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_LENGTH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_LENGTH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SHM_EZCTP_CQ_LENGTH, buf);
	}
#endif

	/* sock_ctrl_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH, buf);
	}
	else {
		/* FIXME: must set sock_ctrl_path */
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH, EZCFG_SOCK_CTRL_PATH);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_CTRL_PATH, EZCFG_SOCK_CTRL_PATH);
	}

	/* sock_nvram_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_NVRAM_PATH, buf);
	}

	/* sock_uevent_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_UEVENT_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_UEVENT_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_UEVENT_PATH, buf);
	}

	/* sock_master_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCK_MASTER_PATH, buf);
	}

	/* web_document_root_path */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_WEB_DOCUMENT_ROOT_PATH);
	DBG("name=[%s]\n", name);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		DBG("%s=%s\n", EZCFG_EZCFG_KEYWORD_WEB_DOCUMENT_ROOT_PATH, buf);
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_WEB_DOCUMENT_ROOT_PATH, buf);
	}

	return EXIT_SUCCESS;
}

static int generate_nvram_conf(FILE *file, int flag, int nvram_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	/* setup nvram storage info */
	for(i = 0; i < nvram_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_NVRAM);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_BUFFER_SIZE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BUFFER_SIZE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_BACKEND_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_BACKEND_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_CODING_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_CODING_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_NVRAM,
		         i, EZCFG_EZCFG_KEYWORD_STORAGE_PATH);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_STORAGE_PATH, buf);
		}
		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}

static int generate_socket_conf(FILE *file, int flag, int socket_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	/* setup socket info */
	for(i = 0; i < socket_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_SOCKET);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_PROTOCOL);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_PROTOCOL, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SOCKET,
		         i, EZCFG_EZCFG_KEYWORD_ADDRESS);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ADDRESS, buf);
		}
		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}

static int generate_auth_conf(FILE *file, int flag, int auth_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	for(i = 0; i < auth_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_AUTH);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_USER);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_USER, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_REALM);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_REALM, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_AUTH,
		         i, EZCFG_EZCFG_KEYWORD_SECRET);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SECRET, buf);
		}
		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
static int generate_ssl_conf(FILE *file, int flag, int ssl_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	for(i = 0; i < ssl_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_SSL);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_ROLE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ROLE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_METHOD);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_METHOD, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_SOCKET_ENABLE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCKET_ENABLE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_SOCKET_DOMAIN);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n",
				EZCFG_EZCFG_KEYWORD_SOCKET_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_SOCKET_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n",
				EZCFG_EZCFG_KEYWORD_SOCKET_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_SOCKET_PROTOCOL);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n",
				EZCFG_EZCFG_KEYWORD_SOCKET_PROTOCOL, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_SSL,
		         i, EZCFG_EZCFG_KEYWORD_SOCKET_ADDRESS);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n",
				EZCFG_EZCFG_KEYWORD_SOCKET_ADDRESS, buf);
		}

		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_IGRS == 1)
static int generate_igrs_conf(FILE *file, int flag, int igrs_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	for(i = 0; i < igrs_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_IGRS);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_IGRS,
		         i, EZCFG_EZCFG_KEYWORD_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_IGRS,
		         i, EZCFG_EZCFG_KEYWORD_USER);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_USER, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_IGRS,
		         i, EZCFG_EZCFG_KEYWORD_REALM);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_REALM, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_IGRS,
		         i, EZCFG_EZCFG_KEYWORD_DOMAIN);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DOMAIN, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_IGRS,
		         i, EZCFG_EZCFG_KEYWORD_SECRET);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SECRET, buf);
		}
		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}
#endif

#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
static int generate_upnp_conf(FILE *file, int flag, int upnp_number)
{
	char name[64];
	char buf[256];
	int rc;
	int i;

	for(i = 0; i < upnp_number; i++) {

		fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_UPNP);

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_UPNP,
		         i, EZCFG_EZCFG_KEYWORD_ROLE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_ROLE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_UPNP,
		         i, EZCFG_EZCFG_KEYWORD_DEVICE_TYPE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DEVICE_TYPE, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_UPNP,
		         i, EZCFG_EZCFG_KEYWORD_DESCRIPTION_PATH);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_DESCRIPTION_PATH, buf);
		}

		snprintf(name, sizeof(name), "%s%s.%d.%s",
		         EZCFG_EZCFG_NVRAM_PREFIX,
		         EZCFG_EZCFG_SECTION_UPNP,
		         i, EZCFG_EZCFG_KEYWORD_INTERFACE);
		if (flag == RC_ACT_BOOT) {
			rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
		}
		else {
			rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
		}
		if (rc >= 0) {
			fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_INTERFACE, buf);
		}

		fprintf(file, "\n");
	}

	return EXIT_SUCCESS;
}
#endif

static int generate_ezcd_conf_file(FILE *file, int flag)
{
	char name[64];
	char buf[256];
	int rc;
	int socket_number = 0;
	int auth_number = 0;
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	int ssl_number = 0;
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRS == 1)
	int igrs_number = 0;
#endif
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	int upnp_number = 0;
#endif

	/* setup ezcfg common info */
	fprintf(file, "[%s]\n", EZCFG_EZCFG_SECTION_COMMON);

	/* log_level */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_LOG_LEVEL);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOG_LEVEL, buf);
	}

	/* setup path info */
	generate_path_conf(file, flag);

	/* socket_number */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SOCKET_NUMBER, buf);
		socket_number = atoi(buf);
	}

	/* locale */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_LOCALE);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_LOCALE, buf);
	}

	/* auth_number */
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_AUTH_NUMBER);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_AUTH_NUMBER, buf);
		auth_number = atoi(buf);
	}

	/* ssl_number */
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_SSL_NUMBER);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_SSL_NUMBER, buf);
		ssl_number = atoi(buf);
	}
#endif

	/* igrs_number */
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRS == 1)
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_IGRS_NUMBER);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_IGRS_NUMBER, buf);
		igrs_number = atoi(buf);
	}
#endif

	/* upnp_number */
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	snprintf(name, sizeof(name), "%s%s.%s",
	         EZCFG_EZCFG_NVRAM_PREFIX,
	         EZCFG_EZCFG_SECTION_COMMON,
	         EZCFG_EZCFG_KEYWORD_UPNP_NUMBER);
	if (flag == RC_ACT_BOOT) {
		rc = utils_get_bootcfg_keyword(name, buf, sizeof(buf));
	}
	else {
		rc = ezcfg_api_nvram_get(name, buf, sizeof(buf));
	}
	if (rc >= 0) {
		fprintf(file, "%s=%s\n", EZCFG_EZCFG_KEYWORD_UPNP_NUMBER, buf);
		upnp_number = atoi(buf);
	}
#endif

	fprintf(file, "\n");

	/* setup nvram storage info */
	generate_nvram_conf(file, flag, EZCFG_NVRAM_STORAGE_NUM);

	/* setup socket info */
	generate_socket_conf(file, flag, socket_number);

	/* setup auth info */
	generate_auth_conf(file, flag, auth_number);

	/* setup ssl info */
#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
	generate_ssl_conf(file, flag, ssl_number);
#endif

	/* setup igrs info */
#if (HAVE_EZBOX_SERVICE_EZCFG_IGRS == 1)
	generate_igrs_conf(file, flag, igrs_number);
#endif

	/* setup upnp info */
#if (HAVE_EZBOX_SERVICE_EZCFG_UPNPD == 1)
	generate_upnp_conf(file, flag, upnp_number);
#endif

	return EXIT_SUCCESS;
}

int pop_etc_ezcd_conf(int flag)
{
	FILE *file;

	switch (flag) {
	case RC_ACT_BOOT :
	case RC_ACT_START :
	case RC_ACT_RELOAD :
	case RC_ACT_RESTART :
		/* get ezcd config from nvram */
		file = fopen(EZCD_CONFIG_FILE_PATH_NEW, "w");
		if (file == NULL)
			return (EXIT_FAILURE);

		generate_ezcd_conf_file(file, flag);

		fclose(file);

		/* rename the new ezcd config file */
		rename(EZCD_CONFIG_FILE_PATH_NEW, EZCD_CONFIG_FILE_PATH);

		break;
	}

	return (EXIT_SUCCESS);
}

/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-ubootenv.c
 *
 * Description  : ezcfg API for u-boot environment parameters manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-10-27   0.1       Write it from scratch
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
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg-api.h"

/**************************************************************************
 *
 * ubootenv design spec
 *
 * 1. the partition name "u-boot-env" indicate that the partition is a
 * u-boot environment data block
 *
 * 2. the partition name "u-boot-env2" indicate that the partition is a
 * redunant u-boot environment data block
 *
 **************************************************************************
 */

/* from u-boot */
/**************************************************************************
 *
 * Support for persistent environment data
 *
 * The "environment" is stored as a list of '\0' terminated
 * "name=value" strings. The end of the list is marked by a double
 * '\0'. New entries are always added at the end. Deleting an entry
 * shifts the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old value and adding the new one.
 *
 * The environment is preceeded by a 32 bit CRC over the data part.
 *
 **************************************************************************
 */

#if 0
#define DBG printf
#else
#define DBG(format, args...)
#endif

typedef struct ubootenv_info_s {
	/* whether uboot env redund */
	bool ubootenv_redundant;

	/* major uboot env */
	unsigned long long ubootenv_size;
	unsigned int ubootenv_erasesize;
	char ubootenv_dev_name[16];

	/* redundant uboot env */
	unsigned long long ubootenv2_size;
	unsigned int ubootenv2_erasesize;
	char ubootenv2_dev_name[16];
} ubootenv_info_t;

static int init_ubootenv_info(ubootenv_info_t *info)
{
	char line[128];
	int idx;
	unsigned long long size;
	int erasesize;
	char name[64];
	FILE *fp = fopen("/proc/mtd", "r");
	if (fp == NULL) {
		return -EZCFG_E_RESOURCE;
	}
	if (fgets(line, sizeof(line), fp) != (char *) NULL) {
		/* read mtd device info */
		for (; fgets(line, sizeof(line), fp);) {
			if (sscanf(line, "mtd%d: %8llx %8x %s\n", &idx, &size, &erasesize, name) != 4) {
				continue;
			}
			if (strcmp(name, "\"u-boot-env\"") == 0) {
				sprintf(info->ubootenv_dev_name, "/dev/mtd%d", idx);
				info->ubootenv_size = size;
				info->ubootenv_erasesize = erasesize;
			}
			if (strcmp(name, "\"u-boot-env2\"") == 0) {
				sprintf(info->ubootenv2_dev_name, "/dev/mtd%d", idx);
				info->ubootenv2_size = size;
				info->ubootenv2_erasesize = erasesize;
				info->ubootenv_redundant = true;
			}
		}
	}
	fclose(fp);
	return 0;
}

static char *find_entry_position(char *data, char *name)
{
	int name_len, entry_len, cmp_len;
	char *entry;

	entry = data;
	name_len = strlen(name);

	while (*entry != '\0') {
		entry_len = strlen(entry) + 1;
		cmp_len = (entry_len > name_len) ? name_len : entry_len ;
		if ((strncmp(name, entry, cmp_len) == 0) &&
		    (*(entry + cmp_len) == '=')) {
			break;
		}
		else {
			entry += entry_len;
		}
	}
	return entry;
}

static int read_ubootenv(ubootenv_info_t info, char *buf, size_t len)
{
	FILE *fp;
	size_t count;

	fp = fopen(info.ubootenv_dev_name, "r");
	if (fp == NULL) {
		return -EZCFG_E_RESOURCE;
	}

	/* read u-boot environment data */
	count = fread(buf, info.ubootenv_size, 1, fp);
	fclose(fp);

	if (count < 1) {
		return -EZCFG_E_RESOURCE;
	}
	else {
		return 0;
	}
}

#if 0
static int erase_ubootenv(ubootenv_info_t info)
{
	int mtd_fd;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	mtd_fd = open(info.ubootenv_dev_name, O_RDWR);
	if (mtd_fd < 0) {
		perror(info.ubootenv_dev_name);
		return errno;
	}

	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(info.ubootenv_dev_name);
		close(mtd_fd);
		return errno;
	}

	erase_info.length = mtd_info.erasesize;
	for (erase_info.start = 0;
	     erase_info.start < mtd_info.size;
	     erase_info.start += mtd_info.erasesize) {
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
			perror(info.ubootenv_dev_name);
			close(mtd_fd);
			return errno;
		}
	}

        close(mtd_fd);

	return 0;
}
#endif

static ezcfg_nv_pair_t default_checklist[] = {
	{ "ethaddr", "0x00:0xaa:0xbb:0xcc:0xdd:0xee" },
	{ "serial#", "########" },
	{ "pin#", "########" },
	{ "model#", "########" },
};
	
static int check_ubootenv_name(char *tmp, char *name, ezcfg_nv_pair_t *checklist, size_t len)
{
	ezcfg_nv_pair_t *cp;
	char *value;
	size_t i = 0;

	for (i = 0; i < len; i++) {
		cp = &(checklist[i]);
		if ((cp->name == NULL) || (cp->value == NULL)){
			continue;
		}
		if (strcmp(cp->name, name) == 0) {
			value = tmp+strlen(name)+1;
			if (strcmp(cp->value, value) != 0) {
				return -1;
			}
		}
	}

	return 0;
}

static int write_ubootenv(ubootenv_info_t info, char *buf, size_t len)
{
	int mtd_fd, count;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	mtd_fd = open(info.ubootenv_dev_name, O_RDWR);
	if (mtd_fd < 0) {
		perror(info.ubootenv_dev_name);
		return -EZCFG_E_RESOURCE;
	}

	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(info.ubootenv_dev_name);
		close(mtd_fd);
		return -EZCFG_E_RESOURCE;
	}

	erase_info.length = mtd_info.erasesize;
	for (erase_info.start = 0;
	     erase_info.start < mtd_info.size;
	     erase_info.start += mtd_info.erasesize) {
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
			perror(info.ubootenv_dev_name);
			close(mtd_fd);
			return -EZCFG_E_RESOURCE;
		}
		count = len > erase_info.length ? erase_info.length : len ;
		if (write(mtd_fd, buf+erase_info.start, count) != count) {
			perror(info.ubootenv_dev_name);
			close(mtd_fd);
			return -EZCFG_E_RESOURCE;
		}
		len -= count;
	}

        close(mtd_fd);

	return 0;
}

/**
 * ezcfg_api_ubootenv_get:
 * @name: u-boot env parameter name
 * @value: buffer to store u-boot env parameter value
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_get(char *name, char *value, size_t len)
{
	int rc = 0;
	char *buf = NULL;
	char *data = NULL, *end = NULL;
	char *tmp = NULL;
	size_t name_len, entry_len, cmp_len;
	uint32_t crc = 0;
	ubootenv_info_t info;

	/* check if name is valid */
	if (name == NULL || value == NULL) {
		return -EZCFG_E_PARSE;
	}
	name_len = strlen(name);
	if (name_len < 1 || len < 1) {
		return -EZCFG_E_PARSE;
	}

	memset(&info, 0, sizeof(info));
	rc = init_ubootenv_info(&info);
	if (rc < 0) {
		goto func_out;
	}

	buf = (char *)malloc(info.ubootenv_size);
	if (buf == NULL) {
		rc = -EZCFG_E_SPACE;
		goto func_out;
	}
	memset(buf, 0, info.ubootenv_size);

	/* read u-boot environment data */
	rc = read_ubootenv(info, buf, info.ubootenv_size);
	if (rc < 0) {
		goto func_out;
	}

	data = buf + sizeof(uint32_t);
	crc = ezcfg_util_crc32((unsigned char *)data, info.ubootenv_size - sizeof(uint32_t));
	crc ^= *(uint32_t *)(buf);
	if (crc != 0) {
		rc = -EZCFG_E_CRC;
		goto func_out;
	}
	crc = *(uint32_t *)(buf);

	/* find \0\0 string */
	end = data+1;
	while (end < (buf + info.ubootenv_size) &&
	       (*(end-1) != '\0' || *end != '\0')) end++;
	if (end == (buf + info.ubootenv_size)) {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

	tmp = find_entry_position(data, name);
	entry_len = strlen(tmp) + 1;
	cmp_len = (entry_len > name_len) ? name_len : entry_len ;
	if (*tmp != '\0' &&
	    strncmp(tmp, name, cmp_len) == 0 &&
	    *(tmp + cmp_len) == '=') {
		/* find the entry */
		entry_len = strlen(tmp) + 1;
		if ((entry_len - (name_len + 1)) > len) {
			rc = -EZCFG_E_SPACE;
			goto func_out;
		}
		snprintf(value, len, "%s", tmp + name_len + 1);
	}
	else {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

func_out:
	if (buf != NULL) {
		free(buf);
	}
	return rc;
}

/**
 * ezcfg_api_ubootenv_set:
 * @name: u-boot env parameter name
 * @value: buffer stored u-boot env parameter value
 *
 **/
int ezcfg_api_ubootenv_set(char *name, char *value)
{
	int rc = 0;
	char *buf = NULL;
	char *data = NULL, *end = NULL;
	char *tmp = NULL;
	int name_len, entry_len, cmp_len, new_entry_len = 0;
	uint32_t crc = 0;
	ubootenv_info_t info;

	/* check if name is valid */
	if (name == NULL) {
		return -EZCFG_E_PARSE;
	}
	name_len = strlen(name);
	if (name_len < 1) {
		return -EZCFG_E_PARSE;
	}

	if (value != NULL) {
		new_entry_len = name_len + strlen(value) + 2;
	}

	memset(&info, 0, sizeof(info));
	rc = init_ubootenv_info(&info);
	if (rc < 0) {
		goto func_out;
	}

	buf = (char *)malloc(info.ubootenv_size);
	if (buf == NULL) {
		rc = -EZCFG_E_SPACE;
		goto func_out;
	}
	memset(buf, 0, info.ubootenv_size);

	/* read u-boot environment data */
	rc = read_ubootenv(info, buf, info.ubootenv_size);
	if (rc < 0) {
		goto func_out;
	}

	data = buf + sizeof(uint32_t);
	crc = ezcfg_util_crc32((unsigned char *)data, info.ubootenv_size - sizeof(uint32_t));
	crc ^= *(uint32_t *)(buf);
	if (crc != 0) {
		rc = -EZCFG_E_CRC;
		goto func_out;
	}
	crc = *(uint32_t *)(buf);

	/* find \0\0 string */
	end = data+1;
	while (end < (buf + info.ubootenv_size) &&
	       (*(end-1) != '\0' || *end != '\0')) end++;
	if (end == (buf + info.ubootenv_size)) {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

	/* first remove the entry */
	tmp = find_entry_position(data, name);
	entry_len = strlen(tmp) + 1;
	cmp_len = (entry_len > name_len) ? name_len : entry_len ;
	if (*tmp != '\0' &&
	    strncmp(tmp, name, cmp_len) == 0 &&
	    *(tmp + cmp_len) == '=') {
		/* check if we can rewrite the entry */
		rc = check_ubootenv_name(tmp, name, default_checklist, ARRAY_SIZE(default_checklist));
		if (rc < 0) {
			goto func_out;
		}
		/* unset the entry */
		memmove(tmp, tmp + entry_len, (end + 1) - (tmp + entry_len));
		/* end pointer move to new \0\0 place */
		end -= entry_len;
	}

	if ((buf + info.ubootenv_size) - end < new_entry_len) {
		/* not enough space */
		rc = -EZCFG_E_SPACE;
		goto func_out;
	}

	/* then add the new entry */
	if (value != NULL) {
		/* add new entry */
		sprintf(end, "%s=%s", name, value);
		/* end pointer move to new \0\0 place */
		end += new_entry_len;
	}

	/* clean remain part */
	memset(end, '\0', info.ubootenv_size - (end - data));
	/* update crc */
	crc = ezcfg_util_crc32((unsigned char *)data, info.ubootenv_size - sizeof(uint32_t));
	*(uint32_t *)(buf) = crc;

	/* write to u-boot env sect */
	rc = write_ubootenv(info, buf, info.ubootenv_size);

func_out:
	if (buf != NULL) {
		free(buf);
	}
	return rc;
}

/**
 * ezcfg_api_ubootenv_list:
 * @list: buffer to store u-boot env parameter pairs
 * @len: buffer size
 *
 **/
int ezcfg_api_ubootenv_list(char *list, size_t len)
{
	int rc = 0;
	char *buf = NULL;
	char *data = NULL, *end = NULL;
	char *tmp = NULL;
	uint32_t crc = 0;
	ubootenv_info_t info;

	if (list == NULL || len < 1) {
		return -EZCFG_E_ARGUMENT ;
	}

	memset(&info, 0, sizeof(info));
	rc = init_ubootenv_info(&info);
	if (rc < 0) {
		goto func_out;
	}

	buf = (char *)malloc(info.ubootenv_size);
	if (buf == NULL) {
		rc = -EZCFG_E_SPACE;
		goto func_out;
	}
	memset(buf, 0, info.ubootenv_size);

	/* read u-boot environment data */
	rc = read_ubootenv(info, buf, info.ubootenv_size);
	if (rc < 0) {
		goto func_out;
	}

	data = buf + sizeof(uint32_t);
	crc = ezcfg_util_crc32((unsigned char *)data, info.ubootenv_size - sizeof(uint32_t));
	crc ^= *(uint32_t *)(buf);
	if (crc != 0) {
		rc = -EZCFG_E_CRC;
		goto func_out;
	}
	crc = *(uint32_t *)(buf);

	/* find \0\0 string */
	end = data+1;
	while (end < (buf + info.ubootenv_size) &&
	       (*(end-1) != '\0' || *end != '\0')) end++;
	if (end == (buf + info.ubootenv_size)) {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

	tmp = data;
	while (len > 1 && tmp != end) {
		*list = *tmp == '\0' ? '\n' : *tmp ;
		list++;
		tmp++;
		len--;
	}
	*list = '\0';

func_out:
	if (buf != NULL) {
		free(buf);
	}
	return rc;
}

/**
 * ezcfg_api_ubootenv_check:
 *
 **/
int ezcfg_api_ubootenv_check(void)
{
	int rc = 0;
	char *buf = NULL;
	char *data = NULL, *end = NULL;
	uint32_t crc = 0;
	ubootenv_info_t info;

	memset(&info, 0, sizeof(info));
	rc = init_ubootenv_info(&info);
	if (rc < 0) {
		goto func_out;
	}

	buf = (char *)malloc(info.ubootenv_size);
	if (buf == NULL) {
		rc = -EZCFG_E_SPACE;
		goto func_out;
	}
	memset(buf, 0, info.ubootenv_size);

	/* read u-boot environment data */
	rc = read_ubootenv(info, buf, info.ubootenv_size);
	if (rc < 0) {
		goto func_out;
	}

	data = buf + sizeof(uint32_t);
	crc = ezcfg_util_crc32((unsigned char *)data, info.ubootenv_size - sizeof(uint32_t));
	crc ^= *(uint32_t *)(buf);
	if (crc != 0) {
		rc = -EZCFG_E_CRC;
		goto func_out;
	}
	crc = *(uint32_t *)(buf);

	/* find \0\0 string */
	end = data+1;
	while (end < (buf + info.ubootenv_size) &&
	       (*(end-1) != '\0' || *end != '\0')) end++;
	if (end == (buf + info.ubootenv_size)) {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

func_out:
	if (buf != NULL) {
		free(buf);
	}
	return rc;
}

/**
 * ezcfg_api_ubootenv_size:
 *
 **/
size_t ezcfg_api_ubootenv_size(void)
{
	int rc = 0;
	ubootenv_info_t info;

	memset(&info, 0, sizeof(info));
	rc = init_ubootenv_info(&info);
	if (rc >= 0) {
		rc = info.ubootenv_size;
	}

	return rc;
}

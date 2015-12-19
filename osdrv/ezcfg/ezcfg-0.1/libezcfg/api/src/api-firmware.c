/* ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-firmware.c
 *
 * Description  : ezcfg API for firmware manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-12-03   0.1       Write it from scratch
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

#if 0
#define DBG printf
#else
#define DBG(format, args...)
#endif

/**************************************************************************
 *
 * firmware design spec
 *
 * 1. The format of a firmware image is as following
 * -----------------------------------------------
 * |fwhdr|model|fwphdr1|...|fwphdrn|fwpart1|...|fwpartn|
 * -----------------------------------------------
 * where :
 * fwhdr is the firmware header structure
 * model is the target model name of the firmware
 * fwphdr1 is the first part header
 * fwphdrn is the n's part header
 * fwpart1 is the first part of data
 * fwpartn is the n's part of data
 *
 **************************************************************************
 */


typedef struct firmware_header_s {
	uint32_t magic; /* "EZFW" indicate an ezbox firmware partition */
	uint32_t len; /* Length of firmware file including header */
	uint32_t crc; /* 32-bit CRC for firmware from model_len */
	uint32_t model_len; /* Model name length */
	uint32_t num_parts; /* Number of parts included in firmware */
} firmware_header_t;

typedef struct firmware_part_s {
	uint32_t len; /* Length of part including part header */
	char name[16]; /* Part name string, fill with '\0', should match with partition name */
} firmware_part_t;

/**
 * get firmware device path
 */
static int get_part_device_path(firmware_part_t *part, char *path, int len)
{
	char line[128];
	int idx;
	unsigned long long size;
	int erasesize;
	char name[32];
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
			sprintf(line, "\"%s\"", part->name);
			if (strcmp(name, line) == 0) {
				snprintf(path, len, "/dev/mtd%d", idx);
				break;
                        }
                }
        }
        fclose(fp);
	return 0;
}

#define PROC_CMDLINE_NAME "/proc/cmdline"
#define MODEL_NAME "model"
static int check_running_model_name(char *model)
{
	FILE * fp;
	char line[1024];
	char *p, *q;
	int ret = 0;

	fp = fopen(PROC_CMDLINE_NAME, "r");
	if (fp == NULL) {
		return -EZCFG_E_RESOURCE;
	}

	if (fgets(line, sizeof(line), fp) == (char *) NULL) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}

	/* parse firmware model name */
	p = strstr(line, MODEL_NAME);
	if (p == NULL) {
		ret = -EZCFG_E_PARSE;
		goto func_out;
	}
	p += strlen(MODEL_NAME); /* skip MODEL_NAME */
	p++; /* skip '=' */
	q = p;
	while (isalnum(*q)) q++;
	*q = '\0';
	if (strcmp(p, model) != 0) {
		ret = -EZCFG_E_RESULT;
		goto func_out;
	}
func_out:
	fclose(fp);
	return ret;
}

#if 0
static int check_firmware_model_name(char *name, char *model)
{
	int fd = -1;
	char buf[32];
	int ret = 0;
	count = 0;
	firmware_header_t fwhdr;

	fd = open(name, O_RDONLY);
	if (fd == -1) {
		return -EZCFG_E_RESOURCE;
	}

	count = read(fd, &fwhdr, sizeof(firmware_header_t));
	if (count != sizeof(firmware_header_t)) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}

	if (fwhdr.model_len > (sizeof(buf) - 1)) {
		/* model name is too long */
		ret = -EZCFG_E_PARSE;
		goto func_out;
	}
	memset(buf, '\0', sizeof(buf));
	count = read(fd, buf, fwhdr.model_len);
	if (count != fwhdr.model_len) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}

	if (strcmp(buf, model) != 0) {
		ret = -EZCFG_E_RESULT;
		goto func_out;
	}
func_out:
	if (fd > 0)
		close(fd);
	if (fwphdr != NULL)
		free(fwphdr);
	return ret;
}
#endif

/* read the firmware header into memory */
static int read_firmware_header(char *name, firmware_header_t *hdr)
{
	int fd = -1;
	int ret = 0;
	int count = 0;

	fd = open(name, O_RDONLY);
	if (fd < 0) {
		return -EZCFG_E_RESOURCE;
	}

	count = read(fd, hdr, sizeof(firmware_header_t));
	if (count != sizeof(firmware_header_t)) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}

func_out:
	if (fd >= 0)
		close(fd);
	return ret;
}

/* read the firmware body into memory */
static int read_firmware_body(char *name, size_t len, char *body)
{
	int fd = -1;
	int ret = 0;
	size_t count = 0;

	fd = open(name, O_RDONLY);
	if (fd < 0) {
		return -EZCFG_E_RESOURCE;
	}

	count = lseek(fd, sizeof(firmware_header_t), SEEK_SET);
	if (count != sizeof(firmware_header_t)) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}

	count = read(fd, body, len);
	if (count != len) {
		ret = -EZCFG_E_RESOURCE;
		goto func_out;
	}
func_out:
	if (fd >= 0)
		close(fd);
	return ret;
}

/* write part data to device */
static int write_part(char *path, char *buf, size_t len)
{
	int mtd_fd, count;
	mtd_info_t mtd_info;
	erase_info_t erase_info;

	mtd_fd = open(path, O_RDWR);
	if (mtd_fd < 0) {
		perror(path);
		return -EZCFG_E_RESOURCE;
	}

	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(path);
		close(mtd_fd);
		return -EZCFG_E_RESOURCE;
	}

	erase_info.length = mtd_info.erasesize;
	for (erase_info.start = 0;
	     erase_info.start < mtd_info.size;
	     erase_info.start += mtd_info.erasesize) {
		(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
		if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
			perror(path);
			close(mtd_fd);
			return -EZCFG_E_RESOURCE;
		}
		count = len > erase_info.length ? erase_info.length : len ;
		if (write(mtd_fd, buf+erase_info.start, count) != count) {
			perror(path);
			close(mtd_fd);
			return -EZCFG_E_RESOURCE;
		}
		len -= count;
	}

	close(mtd_fd);

	return 0;
}

/* write raw firmware file to device */
static int write_firmware(char *path, char *name)
{
	int mtd_fd = -1;
	int fw_fd = 1;
	size_t count;
	ssize_t s_count;
	mtd_info_t mtd_info;
	erase_info_t erase_info;
	char *buf = NULL;
	int rc = 0;

	fw_fd = open(name, O_RDONLY);
	if (fw_fd < 0) {
		perror(path);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	mtd_fd = open(path, O_RDWR);
	if (mtd_fd < 0) {
		perror(path);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	if (ioctl(mtd_fd, MEMGETINFO, &mtd_info) != 0) {
		perror(path);
		rc = -EZCFG_E_RESOURCE ;
		goto func_out;
	}

	erase_info.length = mtd_info.erasesize;
	buf = malloc(erase_info.length);
	if (buf == NULL) {
		rc = -EZCFG_E_SPACE ;
		goto func_out;
	}

	erase_info.start = 0;
	do {
		count = 0;

		/* read a block from firmware file */
		memset(buf, 0, erase_info.length);
		s_count = read(fw_fd, buf, erase_info.length);
		if (s_count > 0) {
			count = s_count;

			/* erase mtd block */
			(void) ioctl(mtd_fd, MEMUNLOCK, &erase_info);
			if (ioctl(mtd_fd, MEMERASE, &erase_info) != 0) {
				perror(path);
				rc = -EZCFG_E_RESOURCE ;
				goto func_out;
			}

			/* write it to mtd block */
			s_count = write(mtd_fd, buf+erase_info.start, count);
			if ((s_count < 0) || ((size_t)s_count != count)) {
				perror(path);
				rc = -EZCFG_E_RESOURCE ;
				goto func_out ;
			}

			/* check next mtd block */
			erase_info.start += erase_info.length;
		}
	} while ((count == erase_info.length) && (erase_info.start < mtd_info.size));

func_out:
	if (buf != NULL) {
		free(buf);
	}

	if (mtd_fd > 0) {
		close(mtd_fd);
	}

	if (fw_fd > 0) {
		close(fw_fd);
	}

	return rc;
}

#define EZCFG_FIRMWARE_MAX_LENGTH 0x1000000 /* 16MB */
/**
 * ezcfg_api_firmware_upgradet:
 * @name: firmware file name
 * @model: firmware model name
 *
 **/
int ezcfg_api_firmware_upgrade(char *name, char *model)
{
	int rc = 0;
	int i;
	char *fw_buf = NULL;
	char *p, *fw_body;
	firmware_header_t *fw_hdr;
	uint32_t crc;
	char path[16];
	firmware_part_t *fw_part, upart;

	/* check if name is valid */
	if (name == NULL) {
		return -EZCFG_E_ARGUMENT ;
	}

	/* if model is not set, write it directly to "firmware" partition */
	if (model == NULL) {
		sprintf(upart.name, "%s", "firmware");
		rc = get_part_device_path(&upart, path, sizeof(path));
		if (rc < 0) {
			return -EZCFG_E_RESOURCE ;
		}
		write_firmware(path, name);
	}

	/* check current running firmware model */
	rc = check_running_model_name(model);
	if (rc < 0) {
		goto func_out;
	}

	fw_buf = (char *)calloc(sizeof(firmware_header_t), 1);
	if (fw_buf == NULL) {
		rc = -EZCFG_E_RESOURCE;
		goto func_out;
	}

	/* read firmware header */
	fw_hdr = (firmware_header_t *)fw_buf;
	rc = read_firmware_header(name, fw_hdr);
	if (rc < 0) {
		goto func_out;
	}

	fw_hdr = (firmware_header_t *)fw_buf;
	if ((fw_hdr->len < sizeof(firmware_header_t)) ||
	    (fw_hdr->len > EZCFG_FIRMWARE_MAX_LENGTH)) {
		rc = -EZCFG_E_PARSE;
		goto func_out;
	}

	p = (char *)realloc(fw_buf, fw_hdr->len);
	if (p == NULL) {
		rc = -EZCFG_E_RESOURCE;
		goto func_out;
	}
	fw_buf = p;
	fw_hdr = (firmware_header_t *)fw_buf;
	fw_body = fw_buf + sizeof(firmware_header_t);

	/* read firmware body */
	rc = read_firmware_body(name, fw_hdr->len - sizeof(firmware_header_t), fw_body);
	if (rc < 0) {
		goto func_out;
	}
	p = (char *)&(fw_hdr->model_len);
	crc = ezcfg_util_crc32((unsigned char *)p, fw_hdr->len - (p - fw_buf));
	crc ^= fw_hdr->crc;
	if (crc != 0) {
		rc = -EZCFG_E_CRC;
		goto func_out;
	}

	/* check upgrading firmware model name */
	if ((strlen(model) != fw_hdr->model_len) ||
	    strncmp(fw_body, model, fw_hdr->model_len) != 0) {
		rc = -EZCFG_E_RESULT ;
		goto func_out;
	}

	/* write firmware part to device */
	p = fw_body + fw_hdr->model_len;
	i = fw_hdr->num_parts;
	while(i > 0) {
		fw_part = (firmware_part_t *)p;
		rc = get_part_device_path(fw_part, path, sizeof(path));
		if (rc < 0) {
			p += fw_part->len;
			i--;
			continue;
		}

		/* write part data to device */
		rc = write_part(path, p + sizeof(firmware_part_t), fw_part->len - sizeof(firmware_part_t));

		p += fw_part->len;
		i--;
	}
func_out:
	if (fw_buf != NULL) {
		free(fw_buf);
	}
	return rc;
}


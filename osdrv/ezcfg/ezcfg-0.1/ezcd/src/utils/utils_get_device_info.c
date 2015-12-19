/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_get_device_info.c
 *
 * Description  : ezcfg get device path function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-06-23   0.1       Write it from scratch
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

#define DEVICE_INFO_PATH_INDEX          1
#define DEVICE_INFO_FS_TYPE_INDEX       2
/*
 * Returns boot device path string
 */
int utils_get_boot_device_path(char *buf, int buf_len)
{
  int rc = -1;
  char *p;
  p = utils_file_get_keyword_by_index("/proc/cmdline", "boot_dev=", DEVICE_INFO_PATH_INDEX);
  if (p != NULL) {
    rc = snprintf(buf, buf_len, "%s", p);
    free(p);
  }
  return rc;
}

int utils_get_boot_device_fs_type(char *buf, int buf_len)
{
  int rc = -1;
  char *p;
  p = utils_file_get_keyword_by_index("/proc/cmdline", "boot_dev=", DEVICE_INFO_FS_TYPE_INDEX);
  if (p != NULL) {
    rc = snprintf(buf, buf_len, "%s", p);
    free(p);
  }
  return rc;
}

/*
 * Returns root device path string
 */
int utils_get_root_device_path(char *buf, int buf_len)
{
  int rc = -1;
  char *p;
  p = utils_file_get_keyword("/proc/cmdline", "root=");
  if (p != NULL) {
    if (strncmp(p, "/dev/", 5) == 0)
      rc = snprintf(buf, buf_len, "%s", p+5);
    else
      rc = snprintf(buf, buf_len, "%s", p);
    free(p);
  }
  return rc;
}

int utils_get_root_device_fs_type(char *buf, int buf_len)
{
  int rc = -1;
  char *p;
  p = utils_file_get_keyword("/proc/cmdline", "rootfstype=");
  if (p != NULL) {
    rc = snprintf(buf, buf_len, "%s", p);
    free(p);
  }
  return rc;
}

/*
 * Returns data device path string
 */
int utils_get_data_device_path(char *buf, int buf_len)
{
  int i;
  struct stat stat_buf;
  int rc = -1;
  char *p;

  for (i = 3; i > 0; i--) {
    if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
      if (S_ISREG(stat_buf.st_mode)) {
	/* get data device path string */
	p = utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH,
                                            NVRAM_SERVICE_OPTION(SYS, DATA_DEV) "=",
                                            DEVICE_INFO_PATH_INDEX);
	if (p != NULL) {
            rc = snprintf(buf, buf_len, "%s", p);
	  free(p);
	}
	return rc;
      }
    }
    /* wait a second then try again */
    sleep(1);
  }
  return rc;
}

int utils_get_data_device_fs_type(char *buf, int buf_len)
{
  int i;
  struct stat stat_buf;
  int rc = -1;
  char *p;

  for (i = 3; i > 0; i--) {
    if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
      if (S_ISREG(stat_buf.st_mode)) {
	/* get data device file system type string */
	p = utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH,
                                            NVRAM_SERVICE_OPTION(SYS, DATA_DEV) "=",
                                            DEVICE_INFO_FS_TYPE_INDEX);
	if (p != NULL) {
	  rc = snprintf(buf, buf_len, "%s", p);
	  free(p);
	}
	return rc;
      }
    }
    /* wait a second then try again */
    sleep(1);
  }
  return rc;
}

int utils_get_hdd_device_path(char *buf, int buf_len)
{
  int i;
  struct stat stat_buf;
  int rc = -1;
  char *p;

  for (i = 3; i > 0; i--) {
    if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
      if (S_ISREG(stat_buf.st_mode)) {
	/* get data device path string */
	p = utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH,
                                            NVRAM_SERVICE_OPTION(SYS, HDD_DEVICE) "=",
                                            DEVICE_INFO_PATH_INDEX);
	if (p != NULL) {
	  rc = snprintf(buf, buf_len, "%s", p);
	  free(p);
	}
	return rc;
      }
    }
    /* wait a second then try again */
    sleep(1);
  }
  return rc;
}

int utils_get_swap_device_path(char *buf, int buf_len)
{
  int i;
  struct stat stat_buf;
  int rc = -1;
  char *p;

  for (i = 3; i > 0; i--) {
    if (stat(BOOT_CONFIG_FILE_PATH, &stat_buf) == 0) {
      if (S_ISREG(stat_buf.st_mode)) {
	/* get data device path string */
	p = utils_file_get_keyword_by_index(BOOT_CONFIG_FILE_PATH,
                                            NVRAM_SERVICE_OPTION(SYS, SWAP_DEV) "=",
                                            DEVICE_INFO_PATH_INDEX);
	if (p != NULL) {
	  rc = snprintf(buf, buf_len, "%s", p);
	  free(p);
	}
	return rc;
      }
    }
    /* wait a second then try again */
    sleep(1);
  }
  return rc;
}

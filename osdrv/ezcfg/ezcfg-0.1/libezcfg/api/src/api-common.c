/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-common.c
 *
 * Description  : ezcfg API for ezcfg common manipulate
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-04   0.1       Write it from scratch
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
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
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

#include "ezcfg.h"
#include "ezcfg-private.h"

#include "ezcfg-api.h"

#if 1
#define DBG(format, args...) \
  do { \
    FILE *dbg_fp = fopen("/tmp/libezcfg-api-common.log", "a");	\
    if (dbg_fp) {						\
      fprintf(dbg_fp, format, ## args);				\
      fclose(dbg_fp);						\
    }								\
  } while(0)
#else
#define DBG(format, args...)
#endif

static bool initialized = false;
static char config_file[EZCFG_PATH_MAX] = EZCFG_CONFIG_FILE_PATH;

/**
 * ezcfg_api_common_initialized:
 *
 **/
bool ezcfg_api_common_initialized(void)
{
  return initialized;
}

/**
 * ezcfg_api_common_get_config_file:
 *
 **/
char *ezcfg_api_common_get_config_file(void)
{
  DBG("%s(%d) config_file=[%s]\n", __func__, __LINE__, config_file);
  return config_file;
}

/**
 * ezcfg_api_common_set_config_file:
 *
 **/
int ezcfg_api_common_set_config_file(const char *path)
{
  int rc = 0;
  size_t len;
  if (path == NULL) {
    return -EZCFG_E_ARGUMENT ;
  }

  len = strlen(path);
  if (len >= sizeof(config_file)) {
    return -EZCFG_E_ARGUMENT ;
  }

  rc = snprintf(config_file, sizeof(config_file), "%s", path);
  if (rc < 0) {
    rc = -EZCFG_E_SPACE ;
  }
  initialized = true;
  return rc;
}

/**
 * ezcfg_api_common_get_config_file_content:
 *
 **/
char *ezcfg_api_common_get_config_file_content(void)
{
  struct stat statbuf;
  char *buf = NULL;
  FILE *fp = NULL;
  size_t read_size = -1;

  DBG("%s(%d) config_file=[%s]\n", __func__, __LINE__, config_file);
  if (stat(config_file, &statbuf) < 0) {
    DBG("%s(%d) can't stat [%s]\n", __func__, __LINE__, config_file);
    return NULL;
  }

  buf = malloc(statbuf.st_size+1);
  if (buf == NULL) {
    DBG("%s(%d) malloc [%lu] bytes error.\n", __func__, __LINE__, statbuf.st_size);
    return NULL;
  }

  fp = fopen(config_file, "r");
  if (fp == NULL) {
    DBG("%s(%d) fopen [%s] bytes error.\n", __func__, __LINE__, config_file);
    free(buf);
    return NULL;
  }

  read_size = fread(buf, sizeof(char), statbuf.st_size, fp);
  DBG("%s(%d) fread return [%ld] bytes.\n", __func__, __LINE__, (long int)read_size);
  DBG("%s(%d) statbuf.st_size [%lu] bytes.\n", __func__, __LINE__, statbuf.st_size);
  if ((statbuf.st_size ^ read_size) != 0) {
    DBG("%s(%d) fread [%s] bytes error.\n", __func__, __LINE__, config_file);
    free(buf);
    buf = NULL;
  }
  else {
    buf[statbuf.st_size] = '\0';
    while (read_size > 0) {
      read_size--;
      if ((buf[read_size] == '\r') ||
          (buf[read_size] == '\n') ||
          (buf[read_size] == '\t') ||
          (buf[read_size] == ' ')) {
        buf[read_size] = '\0';
      }
      else {
        break;
      }
    }
  }
  fclose(fp);
  return buf;
}

/**
 * ezcfg_api_common_new:
 *
 **/
struct ezcfg *ezcfg_api_common_new(char *path)
{
  if (path == NULL) {
    return NULL;
  }
  return ezcfg_new(path);
}

/**
 * ezcfg_api_common_delete:
 *
 **/
void ezcfg_api_common_del(struct ezcfg *ezcfg)
{
  ezcfg_del(ezcfg);
}

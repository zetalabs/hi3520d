/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezcfg Application Programming Interface
 * Module Name  : api-agent.c
 *
 * Description  : ezcfg API for ezcfg agent manipulate
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2013-08-01   0.1       Modify it from api-master.c
 * 2015-06-14   0.2       Reimplement it based on new process/thread objects.
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

static bool debug = false;
static int
log_func(struct ezcfg *ezcfg,
	 int priority,
	 const char *file,
	 int line,
	 const char *func,
	 const char *format,
	 va_list args)
{
  char log_file[EZCFG_PATH_MAX];
  int ret;

  if (debug == false) {
    return EZCFG_RET_OK;
  }

  ret = ezcfg_common_get_log_file(ezcfg, log_file, sizeof(log_file));
  if (ret == EZCFG_RET_FAIL) {
    return EZCFG_RET_FAIL;
  }

  if (log_file[0] == '\0') {
    fprintf(stderr, "libezcfg: %s(%d)@%s: ", file, line, func);
    vfprintf(stderr, format, args);
    return EZCFG_RET_OK;
  }
  else {
    FILE *fp = fopen(log_file, "a");
    if (fp != NULL) {
      fprintf(fp, "libezcfg: %s(%d)@%s: ", file, line, func);
      vfprintf(fp, format, args);
      fclose(fp);
      return EZCFG_RET_OK;
    }
    else {
      return EZCFG_RET_FAIL;
    }
  }
}

/**
 * ezcfg_api_agent_start:
 * @init_conf: ezcfg init configuration
 * @threads_max: max number of threads
 *
 **/
int ezcfg_api_agent_start(char *init_conf)
{
  struct ezcfg *ezcfg = NULL;
  struct ezcfg_socket_agent *agent = NULL;
  int rc = 0;
  int ret = EZCFG_RET_FAIL;
  char *val = NULL;

  if (init_conf == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    return -EZCFG_E_ARGUMENT ;
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  ezcfg = ezcfg_new(init_conf);
  if (ezcfg == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    rc = -EZCFG_E_RESOURCE ;
    goto func_out;
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  ret = ezcfg_common_get_nvram_entry_value(ezcfg, NVRAM_NAME(AGENT, NAME), &val);
  if (ret != EZCFG_RET_OK) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    rc = -EZCFG_E_ARGUMENT ;
    goto func_out;
  }
  if (val) {
    ezcfg_log_init(val);
    ezcfg_common_set_log_func(ezcfg, log_func);
    free(val);
    val = NULL;
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  ret = ezcfg_common_get_nvram_entry_value(ezcfg, NVRAM_NAME(AGENT, NAMESPACE), &val);
  if (ret != EZCFG_RET_OK) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    rc = -EZCFG_E_ARGUMENT ;
    goto func_out;
  }
  agent = ezcfg_socket_agent_new(ezcfg, val);
  if (val) {
    free(val);
    val = NULL;
  }
  if (agent == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    rc = -EZCFG_E_RESOURCE ;
    goto func_out;
  }
  EZDBG("%s(%d)\n", __func__, __LINE__);

  if (EZCFG_RET_OK == ezcfg_socket_agent_start(agent)) {
    rc = 0;
  }
  else {
    ezcfg_socket_agent_stop(agent);
    rc = -EZCFG_E_RESOURCE ;
  }

func_out:
  if (agent)
    ezcfg_socket_agent_del(agent);

  if (ezcfg)
    ezcfg_del(ezcfg);

  return rc;
}

int ezcfg_api_agent_stop(char *init_conf, char *ns)
{
  if (init_conf == NULL) {
    return -EZCFG_E_ARGUMENT ;
  }

  return 0;
}

int ezcfg_api_agent_reload(char *init_conf, char *ns)
{
  if (init_conf == NULL) {
    return -EZCFG_E_ARGUMENT ;
  }

  return 0;
}

int ezcfg_api_agent_set_debug(char *init_conf, char *ns, bool flag)
{
  if (init_conf == NULL) {
    return -EZCFG_E_ARGUMENT ;
  }

  debug = flag;
  return 0;
}


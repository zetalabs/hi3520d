/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_core.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
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
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <pthread.h>
#include <signal.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#if 1
#define DBG(format, args...) \
  do { \
    char path[256];               \
    FILE *dbg_fp;                                           \
    snprintf(path, 256, "/tmp/ezcfg-%d-debug.txt", getpid());     \
    dbg_fp = fopen(path, "a");                              \
    if (dbg_fp) {                                            \
      fprintf(dbg_fp, "tid=[%d] ", (int)gettid());           \
      fprintf(dbg_fp, format, ## args);                      \
      fclose(dbg_fp);                                        \
    }                                                        \
  } while(0)
#else
#define DBG(format, args...)
#endif

/*
 * ezcfg_agent_core:
 *
 * Opaque object handling one event source.
 * Multi-Agents System model - agent part.
 */
struct ezcfg_agent_core {
  struct ezcfg *ezcfg;
  char *name;
  int state;
  struct ezcfg_nvram *nvram; /* Name-Value random access memory */
};

/* Private functions */
/*
 * Deallocate ezcfg agent core state, free up the resources
 */
static void agent_core_delete(struct ezcfg_agent_core *core)
{
  if (core->name != NULL) {
    free(core->name);
  }

  if (core->nvram != NULL) {
    ezcfg_nvram_delete(core->nvram);
  }

  free(core);
}

void ezcfg_agent_core_delete(struct ezcfg_agent_core *core)
{
  if (core == NULL)
    return;

  agent_core_delete(core);
}

/**
 * ezcfg_agent_core_new:
 *
 * Create ezcfg agent core state.
 *
 * Returns: a new ezcfg agent core state structure
 **/
struct ezcfg_agent_core *ezcfg_agent_core_new(struct ezcfg *ezcfg)
{
  struct ezcfg_agent_core *core;

  ASSERT(ezcfg != NULL);

  DBG("%s(%d)\n", __func__, __LINE__);
  core = malloc(sizeof(struct ezcfg_agent_core));
  if (core == NULL) {
    DBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "calloc ezcfg_agent_core fail: %m\n");
    return NULL;
  }

  /* initialize ezcfg library context */
  DBG("%s(%d)\n", __func__, __LINE__);
  memset(core, 0, sizeof(struct ezcfg_agent_core));

  /* set ezcfg library context */
  core->ezcfg = ezcfg;

  /* get nvram memory */
  DBG("%s(%d)\n", __func__, __LINE__);
  core->nvram = ezcfg_nvram_new(ezcfg);
  DBG("%s(%d)\n", __func__, __LINE__);
  if(core->nvram == NULL) {
    DBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "agent core alloc nvram fail: %m\n");
    goto fail_exit;
  }

  /* initialize nvram */
  DBG("%s(%d)\n", __func__, __LINE__);
  if (ezcfg_nvram_initialize(core->nvram) != EZCFG_RET_OK) {
    DBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "agent init nvram fail: %m\n");
    goto fail_exit;
  }

  /* Successfully create agent core state */
  DBG("%s(%d)\n", __func__, __LINE__);
  return core;

fail_exit:
  DBG("%s(%d)\n", __func__, __LINE__);
  agent_core_delete(core);
  DBG("%s(%d)\n", __func__, __LINE__);
  return NULL;
}

struct ezcfg_nvram *ezcfg_agent_core_get_nvram(struct ezcfg_agent_core *core)
{
  return core->nvram;
}

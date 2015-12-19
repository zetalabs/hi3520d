/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 *  ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent.c
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
#include <sys/prctl.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

#ifndef PR_SET_CHILD_SUBREAPER
#define PR_SET_CHILD_SUBREAPER 36
#endif

/*
 * ezcfg_agent:
 *
 * Opaque object handling one event source.
 * Multi-Agents System model - agent part.
 */
struct ezcfg_agent {
  struct ezcfg *ezcfg;
  struct ezcfg_agent_core *core; /* agent state part */
  struct ezcfg_agent_master *master; /* agent act part */
};

/* Private functions */
/*
 * Deallocate ezcfg agent context, free up the resources
 * only delete agent_new() allocated resources before pthread_mutex initialized
 * other resources should be deleted in agent_finish()
 */
static void agent_delete(struct ezcfg_agent *agent)
{
  if (agent == NULL)
    return;

  /* first stop agent act part */
  if (agent->master != NULL) {
    ezcfg_agent_master_stop(agent->master);
  }

  /* then release agent state part */
  if (agent->core != NULL) {
    ezcfg_agent_core_delete(agent->core);
  }

  free(agent);
}

/**
 * agent_new:
 *
 * Create ezcfg agent.
 *
 * Returns: a new ezcfg agent
 **/
static struct ezcfg_agent *agent_new(struct ezcfg *ezcfg)
{
  struct ezcfg_agent *agent;

  ASSERT(ezcfg != NULL);

  agent = malloc(sizeof(struct ezcfg_agent));
  if (agent == NULL) {
    err(ezcfg, "calloc ezcfg_agent fail: %m\n");
    return NULL;
  }

  /* initialize ezcfg library context */
  memset(agent, 0, sizeof(struct ezcfg_agent));

  /* set ezcfg library context */
  agent->ezcfg = ezcfg;

  /* Successfully create agent */
  return agent;
}

/* Public functions */
struct ezcfg_agent *ezcfg_agent_start(struct ezcfg *ezcfg)
{
  struct ezcfg_agent *agent = NULL;

  ASSERT(ezcfg != NULL);

  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (prctl(PR_SET_CHILD_SUBREAPER, 1) < 0) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "failed to make us a subreaper: %m");
    if (errno == EINVAL)
      err(ezcfg, "perhaps the kernel version is too old (< 3.4?)");
    return NULL;
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent = agent_new(ezcfg);
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (agent == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize agent");
    return NULL;
  }

  /* There must be an agent core state */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent->core = ezcfg_agent_core_new(ezcfg);
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (agent->core == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize agent core state");
    goto fail_out;
  }

  /* There must be an agent act executor link with core state*/
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent->master = ezcfg_agent_master_start(ezcfg, agent->core);
  if (agent->master == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize agent act executor");
    goto fail_out;
  }

  /* Successfully create agent */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  return agent;

fail_out:
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent_delete(agent);
  return NULL;
}

void ezcfg_agent_stop(struct ezcfg_agent *agent)
{
  if (agent == NULL)
    return;

  ezcfg_agent_master_stop(agent->master);
}

void ezcfg_agent_reload(struct ezcfg_agent *agent)
{
  if (agent == NULL)
    return;

  ezcfg_agent_master_reload(agent->master);
}

void ezcfg_agent_set_threads_max(struct ezcfg_agent *agent, int threads_max)
{
  if (agent == NULL)
    return;

  ezcfg_agent_master_set_threads_max(agent->master, threads_max);
}

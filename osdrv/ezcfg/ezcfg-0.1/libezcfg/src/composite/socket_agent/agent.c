/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 *  ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : composite/socket_agent/socket_agent.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-29   0.1       Write it from scratch
 * 2015-06-14   0.2       Reimplement it by using process/thread objects
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
struct ezcfg_socket_agent {
  struct ezcfg *ezcfg;
  struct ezcfg_process *process;
  struct ezcfg_thread *master_thread;
  struct ezcfg_thread *worker_threads;

  sigset_t *sigset;

  int stop_flag; /* Should we stop event loop */
  int threads_max; /* MAX number of threads */
  int num_threads; /* Number of threads */
  int num_idle; /* Number of idle threads */

  pthread_mutex_t thread_mutex; /* Protects (max|num)_threads */
  pthread_rwlock_t thread_rwlock; /* Protects options, callbacks */
  pthread_cond_t thread_sync_cond; /* Condvar for thread sync */

  struct ezcfg_socket *listening_sockets;
  pthread_mutex_t ls_mutex; /* Protects listening_sockets */

  struct ezcfg_socket *queue; /* Accepted sockets */
  int sq_len; /* Length of the socket queue */
  int sq_head; /* Head of the socket queue */
  int sq_tail; /* Tail of the socket queue */
  pthread_cond_t sq_empty_cond; /* Socket queue empty condvar */
  pthread_cond_t sq_full_cond;  /* Socket queue full condvar */
};

/* Private functions */
/*
 * Deallocate ezcfg agent context, free up the resources
 * only delete agent_new() allocated resources before pthread_mutex initialized
 * other resources should be deleted in agent_finish()
 */
static void agent_del(struct ezcfg_socket_agent *agent)
{
  if (agent == NULL)
    return;

  /* first stop agent act part */
  if (agent->master != NULL) {
    ezcfg_agent_master_stop(agent->master);
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
static struct ezcfg_socket_agent *agent_new(struct ezcfg *ezcfg)
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

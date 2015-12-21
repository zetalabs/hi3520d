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
 * ezcfg_socket_agent:
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



/* Public functions */
/**
 * ezcfg_socket_agent_new:
 *
 * Create ezcfg agent.
 *
 * Returns: a new ezcfg agent
 **/
struct ezcfg_socket_agent *ezcfg_socket_agent_new(struct ezcfg *ezcfg)
{
  struct ezcfg_socket_agent *agent;

  ASSERT(ezcfg != NULL);

  agent = malloc(sizeof(struct ezcfg_socket_agent));
  if (agent == NULL) {
    err(ezcfg, "calloc ezcfg_socket_agent fail: %m\n");
    return NULL;
  }

  /* initialize ezcfg library context */
  memset(agent, 0, sizeof(struct ezcfg_socket_agent));

  /* There must be an agent process state */
#if 0
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent->process = ezcfg_process_new(ezcfg, ns);
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (agent->process == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize agent process state");
    goto fail_out;
  }
#endif

  /* set ezcfg library context */
  agent->ezcfg = ezcfg;

  /* Successfully create agent */
  return agent;

#if 0
fail_out:
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent_delete(agent);
  return NULL;
#endif
}

/*
 * Deallocate ezcfg agent context, free up the resources
 * only delete agent_new() allocated resources before pthread_mutex initialized
 * other resources should be deleted in agent_finish()
 */
int ezcfg_socket_agent_del(struct ezcfg_socket_agent *agent)
{
  if (agent == NULL)
    return EZCFG_RET_FAIL;

  /* first stop agent act part */
#if 0
  if (agent->master_thread != NULL) {
    ezcfg_socket_agent_master_thread_stop(agent->master_thread);
  }
#endif
  if (agent->worker_threads) {
    free(agent->worker_threads);
    agent->worker_threads = NULL;
  }
  if (agent->master_thread) {
    free(agent->master_thread);
    agent->master_thread = NULL;
  }
  if (agent->process) {
    free(agent->process);
    agent->process = NULL;
  }

  free(agent);
  return EZCFG_RET_OK;
}

int ezcfg_socket_agent_start(struct ezcfg_socket_agent *agent)
{
  struct ezcfg *ezcfg = NULL;

  ASSERT(agent != NULL);

  EZDBG("%s(%d)\n", __func__, __LINE__);
  ezcfg = agent->ezcfg;
  if (prctl(PR_SET_CHILD_SUBREAPER, 1) < 0) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "failed to make us a subreaper: %m");
    if (errno == EINVAL)
      err(ezcfg, "perhaps the kernel version is too old (< 3.4?)");
    return EZCFG_RET_FAIL;
  }

  /* There must be an agent act executor link with core state*/
#if 0
  EZDBG("%s(%d)\n", __func__, __LINE__);
  agent->master = ezcfg_agent_master_start(ezcfg, agent->core);
  if (agent->master == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    err(ezcfg, "can not initialize agent act executor");
    goto fail_out;
  }
#endif

  /* Successfully create agent */
  EZDBG("%s(%d)\n", __func__, __LINE__);
  return EZCFG_RET_OK;
}

int ezcfg_socket_agent_stop(struct ezcfg_socket_agent *agent)
{
  if (agent == NULL)
    return EZCFG_RET_FAIL;

  //ezcfg_socket_agent_master_stop(agent->master);
  return EZCFG_RET_OK;
}

#if 0
void ezcfg_socket_agent_reload(struct ezcfg_agent *agent)
{
  if (agent == NULL)
    return;

  ezcfg_agent_master_reload(agent->master);
}

void ezcfg_socket_agent_set_threads_max(struct ezcfg_socket_agent *agent, int threads_max)
{
  if (agent == NULL)
    return;

  ezcfg_agent_master_set_threads_max(agent->master, threads_max);
}
#endif

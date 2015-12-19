/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/thread/thread.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * 2015-06-10   0.2       Reimplement it
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
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

/* bitmap for thread state */
#define THREAD_STATE_STOPPED     0
#define THREAD_STATE_RUNNING     1
#define THREAD_STATE_STOPPING    2

/*
 * ezcfg-thread - ezbox config multi-threads model
 *
 */

struct ezcfg_thread {
  struct ezcfg *ezcfg;
  int state;
  pthread_t thread_id;
  pthread_attr_t attr;
  void *(*start_routine)(void *);
  void *arg;
  int (*arg_del_handler)(void *);
  int (*stop)(struct ezcfg_thread *);
};

/* mutex for thread_state manipulate */
static pthread_mutex_t thread_state_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * private functions
 */
static int lock_thread_state_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_lock(&(thread_state_mutex)) < 0) &&
         (retry < EZCFG_LOCK_RETRY_MAX)) {
    my_errno = errno;
    EZDBG("%s(%d) pthread_mutex_lock() with errno=[%d]\n", __func__, __LINE__, my_errno);
    if (my_errno == EINVAL) {
      return EZCFG_RET_FAIL;
    }
    else {
      EZDBG("%s(%d) wait a second then try again...\n", __func__, __LINE__);
      sleep(1);
      retry++;
    }
  }
  if (retry < EZCFG_LOCK_RETRY_MAX) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

static int unlock_thread_state_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_unlock(&(thread_state_mutex)) < 0) &&
         (retry < EZCFG_UNLOCK_RETRY_MAX)) {
    my_errno = errno;
    EZDBG("%s(%d) pthread_mutex_unlock() with errno=[%d]\n", __func__, __LINE__, my_errno);
    if (my_errno == EINVAL) {
      return EZCFG_RET_FAIL;
    }
    else {
      EZDBG("%s(%d) wait a second then try again...\n", __func__, __LINE__);
      sleep(1);
      retry++;
    }
  }
  if (retry < EZCFG_UNLOCK_RETRY_MAX) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}


/**
 * public functions
 */

struct ezcfg_thread *ezcfg_thread_new(struct ezcfg *ezcfg, char *ns)
{
  struct ezcfg_thread *thread = NULL;
  int detachstate = PTHREAD_CREATE_DETACHED;
  int stacksize = 0;
  char name[EZCFG_NAME_MAX] = "";
  struct ezcfg_linked_list *list = NULL;
  struct ezcfg_nv_pair *data = NULL;
  int ret = EZCFG_RET_FAIL;
  int i = 0, list_length = 0;
  char *p_detachstate = NULL;
  char *p_stacksize = NULL;
  int s = -1;
  char *val = NULL;

  ASSERT (ezcfg != NULL);

  /* increase ezcfg library context reference */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_inc_ref() failed\n");
    return NULL;
  }

  thread = (struct ezcfg_thread *)calloc(1, sizeof(struct ezcfg_thread));
  if (thread == NULL) {
    err(ezcfg, "can not calloc thread\n");
    goto exit_fail;
  }

  /* get thread attributes */
  list = ezcfg_linked_list_new(ezcfg,
    ezcfg_nv_pair_del_handler,
    ezcfg_nv_pair_cmp_handler);
  if (list == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    goto exit_fail;
  }

  /* detachstate */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(THREAD, DETACHSTATE));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  p_detachstate = ezcfg_nv_pair_get_n(data);
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = NULL;

  /* stacksize */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, NVRAM_NAME(THREAD, STACKSIZE));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = ezcfg_nv_pair_new(name, NULL);
  if (data == NULL) {
    goto exit_fail;
  }
  p_stacksize = ezcfg_nv_pair_get_n(data);
  if (ezcfg_linked_list_append(list, data) != EZCFG_RET_OK) {
    goto exit_fail;
  }
  data = NULL;

  /* get these value from nvram */
  ret = ezcfg_common_get_nvram_entries(ezcfg, list);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }

  /* parse settings */
  list_length = ezcfg_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (struct ezcfg_nv_pair *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      goto exit_fail;
    }
    if (ezcfg_nv_pair_get_n(data) == p_detachstate) {
      val = ezcfg_nv_pair_get_v(data);
      if (val == NULL) {
        goto exit_fail;
      }
      detachstate = atoi(val);
    }
    else if (ezcfg_nv_pair_get_n(data) == p_stacksize) {
      val = ezcfg_nv_pair_get_v(data);
      if (val == NULL) {
        goto exit_fail;
      }
      stacksize = atoi(val);
    }
  }
  data = NULL;

  /* cleanup list */
  ezcfg_linked_list_del(list);
  list = NULL;

  s = pthread_attr_init(&(thread->attr));
  if (s != 0) {
    err(ezcfg, "%s: pthread_attr_init %s", __func__, strerror(s));
    goto exit_fail;
  }

  s = pthread_attr_setdetachstate(&(thread->attr), detachstate);
  if (s != 0) {
    err(ezcfg, "%s: pthread_attr_setdetachstate %s", __func__, strerror(s));
    goto exit_fail;
  }

  if (stacksize > 0) {
    s = pthread_attr_setstacksize(&(thread->attr), stacksize);
    if (s != 0) {
      err(ezcfg, "%s: %s", __func__, strerror(s));
      goto exit_fail;
    }
  }

  thread->state = THREAD_STATE_STOPPED;
  thread->ezcfg = ezcfg;

  return thread;

exit_fail:
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (data) {
    ezcfg_nv_pair_del(data);
  }
  if (list) {
    ezcfg_linked_list_del(list);
  }
  if (thread != NULL) {
    free(thread);
  }
  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_dec_ref() failed\n");
  }
  return NULL;
}

int ezcfg_thread_del(struct ezcfg_thread *thread)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(thread != NULL);

  if (lock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("lock_thread_state_mutex() failed\n");
    return EZCFG_RET_FAIL;
  }

  if (thread->state != THREAD_STATE_STOPPED) {
    EZDBG("thread must stop first\n");
    if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
      EZDBG("unlock_thread_state_mutex() failed\n");
    }
    return EZCFG_RET_FAIL;
  }

  if (thread->arg) {
    if (thread->arg_del_handler) {
      ret = thread->arg_del_handler(thread->arg);
    }
    else {
      free(thread->arg);
      ret = EZCFG_RET_OK;
    }
    if (ret != EZCFG_RET_OK) {
      if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
        EZDBG("unlock_thread_state_mutex() failed\n");
      }
      return EZCFG_RET_FAIL;
    }
  }

  free(thread);
  if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("unlock_thread_state_mutex() failed\n");
  }
  return EZCFG_RET_OK;
}

int ezcfg_thread_set_start_routine(
  struct ezcfg_thread *thread,
  void *(*func)(void *))
{
  ASSERT(thread != NULL);
  ASSERT(func != NULL);

  thread->start_routine = func;
  return EZCFG_RET_OK;
}

int ezcfg_thread_set_arg_del_handler(
  struct ezcfg_thread *thread,
  int (*func)(void *))
{
  ASSERT(thread != NULL);
  ASSERT(func != NULL);

  thread->arg_del_handler = func;
  return EZCFG_RET_OK;
}

int ezcfg_thread_set_stop(
  struct ezcfg_thread *thread,
  int (*func)(struct ezcfg_thread *))
{
  ASSERT(thread != NULL);
  ASSERT(func != NULL);

  thread->stop = func;
  return EZCFG_RET_OK;
}

int ezcfg_thread_start(struct ezcfg_thread *thread)
{
  struct ezcfg *ezcfg = NULL;
  int s = -1;

  ASSERT(thread != NULL);
  ASSERT(thread->start_routine != NULL);

  ezcfg = thread->ezcfg;

  if (lock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("lock_thread_state_mutex() failed\n");
    return EZCFG_RET_FAIL;
  }

  if (thread->state != THREAD_STATE_STOPPED) {
    EZDBG("thread must stop first\n");
    if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
      EZDBG("unlock_thread_state_mutex() failed\n");
    }
    return EZCFG_RET_FAIL;
  }

  s = pthread_create(&(thread->thread_id), &(thread->attr), thread->start_routine, thread->arg);
  if (s != 0) {
    err(ezcfg, "%s: %s", __func__, strerror(s));
    if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
      EZDBG("unlock_thread_state_mutex() failed\n");
    }
    return EZCFG_RET_FAIL;
  }

  thread->state = THREAD_STATE_RUNNING;

  if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("unlock_thread_state_mutex() failed\n");
  }
  return EZCFG_RET_OK;
}

int ezcfg_thread_stop(struct ezcfg_thread *thread)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(thread != NULL);

  if (lock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("lock_thread_state_mutex() failed\n");
    return EZCFG_RET_FAIL;
  }

  if (thread->state == THREAD_STATE_STOPPED) {
    EZDBG("thread has already stopped\n");
    ret = EZCFG_RET_OK;
    goto func_exit;
  }

  if (thread->state != THREAD_STATE_RUNNING) {
    EZDBG("thread must stop first\n");
    ret = EZCFG_RET_FAIL;
    goto func_exit;
  }

  if (thread->stop) {
    ret = thread->stop(thread);
    if (ret == EZCFG_RET_OK) {
      thread->state = THREAD_STATE_STOPPING;
    }
    else {
      ret = EZCFG_RET_FAIL;
    }
  }
  else {
    ret = EZCFG_RET_FAIL;
  }

func_exit:
  if (unlock_thread_state_mutex() != EZCFG_RET_OK) {
    EZDBG("unlock_thread_state_mutex() failed\n");
  }
  return ret;
}


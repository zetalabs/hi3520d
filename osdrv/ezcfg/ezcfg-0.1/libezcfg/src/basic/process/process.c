/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/process/process.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-11   0.1       Write it from scratch
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

/* bitmap for process state */
#define PROCESS_STATE_STOPPED     0
#define PROCESS_STATE_RUNNING     1
#define PROCESS_STATE_STOPPING    2

/*
 * ezcfg-process - ezbox config process model
 *
 */

struct ezcfg_process {
  struct ezcfg *ezcfg;
  int state;
  pid_t process_id; /* process's own pid */
};

/* mutex for thread_state manipulate */
static pthread_mutex_t process_mutex = PTHREAD_MUTEX_INITIALIZER;

//static struct ezcfg_process *this_process = NULL;
//static struct ezcfg_process *parent_process = NULL;
static struct ezcfg_linked_list *child_process_list = NULL;

/**
 * private functions
 */
static int lock_process_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_lock(&(process_mutex)) < 0) &&
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

static int unlock_process_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_unlock(&(process_mutex)) < 0) &&
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

static int process_start(char *filename, char *initconf)
{
  char *new_argv[] = { filename, initconf, NULL };
  char *new_env[] = { NULL };

  return execve(filename, new_argv, new_env);
}

/**
 * public functions
 */

int ezcfg_process_new(struct ezcfg *ezcfg, char *ns)
{
  struct ezcfg_process *process = NULL;
  pid_t cpid = -1;
  int my_errno = 0;
  char *child_filename = NULL;
  char *child_initconf = NULL;
  char name[EZCFG_NAME_MAX] = "";
  struct ezcfg_linked_list *list = NULL;
  struct ezcfg_linked_list *new_list = NULL;
  int ret = EZCFG_RET_FAIL;
  struct ezcfg_nv_pair *data = NULL;
  struct ezcfg_nv_pair *new_data = NULL;
  int i = 0, list_length = 0;
  int sub_ns_len = 0;
  int meta_nvram_prefix_len = 0;
  char *p_name = NULL;
  char *p_value = NULL;

  ASSERT (ezcfg != NULL);

  meta_nvram_prefix_len = strlen(EZCFG_NVRAM_PREFIX_META);

  /* build sub-namespace for process */
  ret = ezcfg_util_snprintf_ns_name(name, sizeof(name), ns, EZCFG_NVRAM_PREFIX_PROCESS);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  sub_ns_len = strlen(name);

  ret = ezcfg_common_get_nvram_entries_by_ns(ezcfg, name, &list);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }

  /* parse settings */
  new_list = ezcfg_linked_list_new(ezcfg,
    ezcfg_nv_pair_del_handler,
    ezcfg_nv_pair_cmp_handler);
  if (new_list == NULL) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    goto exit_fail;
  }

  list_length = ezcfg_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (struct ezcfg_nv_pair *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      goto exit_fail;
    }
    p_name = ezcfg_nv_pair_get_n(data) + sub_ns_len;
    if (strcmp(p_name, EZCFG_NVRAM_PROCESS_FILENAME) == 0) {
      child_filename = strdup(ezcfg_nv_pair_get_v(data));
      if (child_filename == NULL) {
        goto exit_fail;
      }
    }
    else if (strncmp(p_name, EZCFG_NVRAM_PREFIX_META, meta_nvram_prefix_len) == 0) {
      p_value = ezcfg_nv_pair_get_v(data);
      data = NULL;
      new_data = ezcfg_nv_pair_new(p_name, p_value);
      if (new_data == NULL) {
        goto exit_fail;
      }
      if (ezcfg_linked_list_append(new_list, new_data) != EZCFG_RET_OK) {
        goto exit_fail;
      }
      new_data = NULL;
    }
  }
  data = NULL;

  /* build initconf for child process */
  if (new_list) {
    child_initconf = ezcfg_linked_list_nv_pair_to_json_text(new_list);
    if (child_initconf == NULL) {
      err(ezcfg, "ezcfg_linked_list_nv_pair_to_json_text() failed.\n");
      goto exit_fail;
    }
    ezcfg_linked_list_del(new_list);
    new_list = NULL;
  }

  process = (struct ezcfg_process *)calloc(1, sizeof(struct ezcfg_process));
  if (process == NULL) {
    err(ezcfg, "can not calloc process\n");
    goto exit_fail;
  }

  cpid = fork();
  if (cpid == -1) {
    /* it's in original process runtime space */
    my_errno = errno;
    err(ezcfg, "can not fork, errno=[%d]\n", my_errno);
    goto exit_fail;
  }

  if (cpid == 0) {
    /* it's in child process runtime space */
    /* cleanup parent process resources */
    pthread_mutex_init(&process_mutex, NULL);
    child_process_list = NULL;

    /* child process must stay in this block!!! */
    return process_start(child_filename, child_initconf);
  }
  else {
    /* it's in parent process runtime space */
    /* cleanup unused variables */
    if (child_filename) {
      free(child_filename);
      child_filename = NULL;
    }
    if (child_initconf) {
      free(child_initconf);
      child_initconf = NULL;
    }
    /* set child process info */
    process->state = PROCESS_STATE_STOPPED;
    process->process_id = cpid;
    process->ezcfg = ezcfg;
    /* add child process info to child_process_list */
    if (lock_process_mutex() != EZCFG_RET_OK) {
      EZDBG("lock_process_mutex() failed\n");
      goto exit_fail;
    }
    if (child_process_list == NULL) {
      child_process_list = ezcfg_linked_list_new(ezcfg,
        ezcfg_process_del_handler,
        ezcfg_process_cmp_handler);
      if (child_process_list == NULL) {
        EZDBG("%s(%d)\n", __func__, __LINE__);
        if (unlock_process_mutex() != EZCFG_RET_OK) {
          EZDBG("unlock_process_mutex() failed\n");
        }
        goto exit_fail;
      }
    }
    if (ezcfg_linked_list_append(child_process_list, process) != EZCFG_RET_OK) {
      EZDBG("%s(%d)\n", __func__, __LINE__);
      if (unlock_process_mutex() != EZCFG_RET_OK) {
        EZDBG("unlock_process_mutex() failed\n");
      }
      goto exit_fail;
    }
  }

  EZDBG("%s(%d)\n", __func__, __LINE__);
  return EZCFG_RET_OK;

exit_fail:
  EZDBG("%s(%d)\n", __func__, __LINE__);
  if (child_filename) {
    free(child_filename);
  }
  if (child_initconf) {
    free(child_initconf);
  }
  if (data) {
    ezcfg_nv_pair_del(data);
  }
  if (list) {
    ezcfg_linked_list_del(list);
  }
  if (new_data) {
    ezcfg_nv_pair_del(new_data);
  }
  if (new_list) {
    ezcfg_linked_list_del(new_list);
  }
  if (process != NULL) {
    free(process);
  }
  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_dec_ref() failed\n");
  }
  return EZCFG_RET_FAIL;
}

int ezcfg_process_del(struct ezcfg_process *process)
{
  ASSERT(process != NULL);

  if (lock_process_mutex() != EZCFG_RET_OK) {
    EZDBG("lock_thread_state_mutex() failed\n");
    return EZCFG_RET_FAIL;
  }

  if (process->state != PROCESS_STATE_STOPPED) {
    EZDBG("process must stop first\n");
    if (unlock_process_mutex() != EZCFG_RET_OK) {
      EZDBG("unlock_process_mutex() failed\n");
    }
    return EZCFG_RET_FAIL;
  }

  /* cleanup child process list */
  if (child_process_list) {
    ezcfg_linked_list_del(child_process_list);
  }

  free(process);
  if (unlock_process_mutex() != EZCFG_RET_OK) {
    EZDBG("unlock_process_mutex() failed\n");
  }
  return EZCFG_RET_OK;
}

int ezcfg_process_del_handler(void *data)
{
  ASSERT(data != NULL);
  return ezcfg_process_del((struct ezcfg_process *)data);
}

int ezcfg_process_cmp_handler(const void *d1, const void *d2)
{
  struct ezcfg_process *p1 = NULL;
  struct ezcfg_process *p2 = NULL;

  ASSERT(d1 != NULL);
  ASSERT(d2 != NULL);

  p1 = (struct ezcfg_process *)d1;
  p2 = (struct ezcfg_process *)d2;
  if (p1->process_id == p2->process_id) {
    return 0;
  }
  else {
    return -1;
  }
}

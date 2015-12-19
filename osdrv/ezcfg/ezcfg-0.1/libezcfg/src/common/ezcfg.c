/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
 * 2014-03-30   0.2       Use meta NVRAM as the raw representation
 * 2015-06-08   0.3       Add NVRAM for self-gaining representation
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

#include "ezcfg_local.h"

/* these functions shall be called by ezcfg only !!! */
extern int _local_meta_nvram_set_entries(char *buffer, struct linked_list *list);
extern int _local_meta_nvram_get_entries_by_ns(char *buffer, char *ns, struct ezcfg_linked_list *list);
extern int _local_nvram_mutex_lock(struct nvram *nvram);
extern int _local_nvram_mutex_unlock(struct nvram *nvram);
extern int _local_nvram_set_entry(struct nvram *nvram, const char *name, const char *value);
extern int _local_nvram_get_entry_value(struct nvram *nvram, const char *name, char **value);
extern int _local_nvram_unset_entry(struct nvram *nvram, const char *name);
extern int _local_nvram_get_node_list_size(struct nvram *nvram);
extern int _local_nvram_get_entries_by_ns(struct nvram *nvram, char *ns, struct ezcfg_linked_list *list);


/*
 * ezcfg - library context
 *
 * load/save the ezbox config and system environment
 * allows custom logging
 */

/* mutex for increasing/decreasing ezcfg_ref_counter */
static pthread_mutex_t ezcfg_ref_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ezcfg_ref_counter = 0;
static struct ezcfg *this_ezcfg = NULL;

/*
 * ezbox config context
 */
struct ezcfg
{
  int (*log_func)(struct ezcfg *ezcfg,
                  int priority,
                  const char *file,
                  int line,
                  const char *func,
                  const char *format,
                  va_list args);
  char log_file[EZCFG_PATH_MAX];
  char *meta_nvram; /* read-only, inherited from outside */
  struct nvram *nvram; /* read-writeable, gained by oneself */
};

/**
 * private functions
 */

static int
log_stderr(struct ezcfg *ezcfg,
           int priority,
           const char *file,
           int line,
           const char *func,
           const char *format,
           va_list args)
{
  if (ezcfg->log_file[0] == '\0') {
    fprintf(stderr, "libezcfg: %s(%d)@%s: ", file, line, func);
    vfprintf(stderr, format, args);
    return EZCFG_RET_OK;
  }
  else {
    FILE *fp = fopen(ezcfg->log_file, "a");
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

static int lock_ezcfg_ref_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_lock(&(ezcfg_ref_mutex)) < 0) &&
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

static int unlock_ezcfg_ref_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_unlock(&(ezcfg_ref_mutex)) < 0) &&
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

/**
 * ezcfg_common_set_log_func:
 * @ezcfg: ezcfg library context
 * @log_func: function to be called for logging messages
 *
 * The built-in logging writes to stderr. It can be
 * overridden by a custom function, to plug log messages
 * into the users' logging functionality.
 *
 */
int
ezcfg_common_set_log_func(struct ezcfg *ezcfg,
                          int (*log_func)(struct ezcfg *ezcfg,
                                          int priority,
                                          const char *file,
                                          int line,
                                          const char *func,
                                          const char *format,
                                          va_list args))
{
  ezcfg->log_func = log_func;
  return EZCFG_RET_OK;
}

/**
 * ezcfg_new:
 *
 * @text: META NVRAM string by JSON representing
 *
 * Create ezcfg library context.
 *
 * Returns: a new ezcfg library context
 **/
struct ezcfg *ezcfg_new(char *text)
{
  struct ezcfg *ezcfg = NULL;
  struct json *json = NULL;
  struct linked_list *list = NULL;
  int size = 0;
  int ret = EZCFG_RET_FAIL;

  ASSERT (this_ezcfg == NULL);

  EZDBG("%s(%d) text=[%s]\n", __func__, __LINE__, text);

  ezcfg = malloc(sizeof(struct ezcfg));
  if (ezcfg == NULL) {
    return NULL;
  }

  /* increase ezcfg_ref_counter */
  if (lock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    free(ezcfg);
    return NULL;
  }

  if (ezcfg_ref_counter != 0) {
    EZDBG("%s(%d) ezcfg_ref_counter=[%d] is invalid, it should be 0!\n", __func__, __LINE__, ezcfg_ref_counter);
    if (unlock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
      EZDBG("%s(%d)\n", __func__, __LINE__);
    }
    free(ezcfg);
    return NULL;
  }

  ezcfg_ref_counter++;

  if (unlock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    EZDBG("%s(%d)\n", __func__, __LINE__);
    free(ezcfg);
    return NULL;
  }

  ezcfg->log_func = log_stderr;
  ezcfg->log_file[0] = '\0';
  ezcfg->meta_nvram = NULL;
  ezcfg->nvram = NULL;

  if (text != NULL) {
    /* check text is the meta list format */
    json = local_json_new();
    if (json == NULL) {
      goto fail_out;
    }
    if (local_json_parse_text(json, text, strlen(text)) != EZCFG_RET_OK) {
      goto fail_out;
    }
    size = local_json_get_msg_len(json);
    size += sizeof(struct nvram_header);
    ezcfg->meta_nvram = local_meta_nvram_new(size);
    if (ezcfg->meta_nvram == NULL) {
      goto fail_out;
    }
    /* setup meta nvram */
    if (local_json_is_nvram_representation(json) == true) {
      list = local_json_build_nvram_node_list(json);
      if (list != NULL) {
        ret = _local_meta_nvram_set_entries(ezcfg->meta_nvram, list);
        local_linked_list_del(list);
        list = NULL;
        if (ret == EZCFG_RET_FAIL) {
          goto fail_out;
        }
      }
    }
    /* setup nvram */
    ezcfg->nvram = local_nvram_new(ezcfg->meta_nvram);
    if (ezcfg->nvram == NULL) {
      EZDBG("%s(%d) no ezcfg nvram.\n", __func__, __LINE__);
    }
  }

  /* new ezcfg OK! */
  this_ezcfg = ezcfg;
  return ezcfg;

fail_out:
  if (json != NULL) {
    local_json_del(json);
  }
  if (ezcfg != NULL) {
    if (ezcfg->meta_nvram != NULL) {
      local_meta_nvram_del(ezcfg->meta_nvram);
    }
    ezcfg_del(ezcfg);
  }
  return NULL;
}

/**
 * ezcfg_del:
 * @ezcfg: ezcfg library context
 *
 * Release the ezcfg library context.
 *
 **/
int ezcfg_del(struct ezcfg *ezcfg)
{
  int ret = EZCFG_RET_FAIL;
  int retry = 0;
  struct nvram *nvram = NULL;
  char *meta_nvram = NULL;

  ASSERT (ezcfg != NULL);
  //ASSERT (this_ezcfg != NULL);
  ASSERT (this_ezcfg == ezcfg);

  if (lock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    return EZCFG_RET_FAIL;
  }

  while ((ezcfg_ref_counter != 1) &&
         (retry < EZCFG_REF_COUNTER_RETRY_MAX)) {
    EZDBG("%s(%d) ezcfg_ref_counter=[%d]\n", __func__, __LINE__, ezcfg_ref_counter);
    if (ezcfg_ref_counter > 1) {
      EZDBG("%s(%d) wait a second then try again...\n", __func__, __LINE__);
      sleep(1);
      retry++;
    }
    else {
      EZDBG("%s(%d) ezcfg_ref_counter is invalid, return now!\n", __func__, __LINE__);
      ret = EZCFG_RET_FAIL;
      goto func_exit;
    }
  }

  if (retry < EZCFG_REF_COUNTER_RETRY_MAX) {
    /* first tag it as being unavailable */
    ezcfg_ref_counter--;
    this_ezcfg = NULL;

    /* then cleanup ezcfg */
    if (ezcfg->nvram != NULL) {
      nvram = ezcfg->nvram;
      ezcfg->nvram = NULL;
      local_nvram_del(nvram);
    }
    if (ezcfg->meta_nvram != NULL) {
      meta_nvram = ezcfg->meta_nvram;
      ezcfg->meta_nvram = NULL;
      local_meta_nvram_del(meta_nvram);
    }
    free(ezcfg);

    ret = EZCFG_RET_OK;
  }
  else {
    ret = EZCFG_RET_FAIL;
  }

func_exit:
  if (unlock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    ret = EZCFG_RET_FAIL;
  }

  return ret;
}

/**
 * ezcfg_increase_ref:
 *
 * Increase ezcfg reference counter
 *
 **/
int ezcfg_inc_ref(struct ezcfg *ezcfg)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT (ezcfg != NULL);
  //ASSERT (this_ezcfg != NULL);
  ASSERT (this_ezcfg == ezcfg);

  if (lock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    return EZCFG_RET_FAIL;
  }

  /* ezcfg must have been created before! */
  if (ezcfg_ref_counter > 0) {
    ezcfg_ref_counter++;
    ret = EZCFG_RET_OK;
  }
  else {
    /* error! the reference to ezcfg must be at least one! */
    ret = EZCFG_RET_FAIL;
  }

  if (unlock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
      ret = EZCFG_RET_FAIL;
  }

  return ret;
}

/**
 * ezcfg_dec_ref:
 *
 * Decrease ezcfg reference counter
 *
 **/
int ezcfg_dec_ref(struct ezcfg *ezcfg)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT (ezcfg != NULL);
  //ASSERT (this_ezcfg != NULL);
  ASSERT (this_ezcfg == ezcfg);

  if (lock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
    return EZCFG_RET_FAIL;
  }

  /* ezcfg must have been created before! */
  if (ezcfg_ref_counter > 1) {
    ezcfg_ref_counter--;
    ret = EZCFG_RET_OK;
  }
  else {
    /* error! the reference to ezcfg must be more than one! */
    ret = EZCFG_RET_FAIL;
  }

  if (unlock_ezcfg_ref_mutex() != EZCFG_RET_OK) {
      ret = EZCFG_RET_FAIL;
  }

  return ret;
}

int
ezcfg_log(struct ezcfg *ezcfg,
          int priority,
          const char *file,
          int line,
          const char *func,
          const char *format,
          ...)
{
  va_list args;

  va_start(args, format);
  ezcfg->log_func(ezcfg, priority, file, line, func, format, args);
  va_end(args);
  return EZCFG_RET_OK;
}

int ezcfg_common_get_log_file(struct ezcfg *ezcfg, char *buf, size_t size)
{
  if (size <= strlen(ezcfg->log_file)) {
    return EZCFG_RET_FAIL;
  }
  snprintf(buf, size, "%s", ezcfg->log_file);
  return EZCFG_RET_OK;
}

int ezcfg_common_set_log_file(struct ezcfg *ezcfg, char *buf)
{
  if (strlen(buf) < sizeof(ezcfg->log_file)) {
    snprintf(ezcfg->log_file, sizeof(ezcfg->log_file), "%s", buf);
    return EZCFG_RET_OK;
  }
  return EZCFG_RET_FAIL;
}

int ezcfg_common_get_log_priority(struct ezcfg *ezcfg)
{
  return LOG_ERR;
}

int ezcfg_common_get_nvram_entry_value(struct ezcfg *ezcfg, const char *name, char **pvalue)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT (ezcfg != NULL);
  ASSERT (name != NULL);
  ASSERT (pvalue != NULL);

  if (ezcfg->nvram) {
    ret = local_nvram_get_entry_value(ezcfg->nvram, name, pvalue);
  }
  if (ret != EZCFG_RET_OK) {
    ret = local_meta_nvram_get_entry_value(ezcfg->meta_nvram, name, pvalue);
  }
  return ret;
}

int ezcfg_common_get_nvram_entries(struct ezcfg *ezcfg, struct ezcfg_linked_list *list)
{
  struct ezcfg_nv_pair *data = NULL;
  int i, list_length;
  int ret = EZCFG_RET_FAIL;
  int nvram_mutex_locked = 0;
  struct nvram *nvram = NULL;

  ASSERT (ezcfg != NULL);
  ASSERT (list != NULL);

  nvram = ezcfg->nvram;

  list_length = ezcfg_linked_list_get_length(list);

  if ((list_length > 0) && (nvram != NULL)) {
    ret = _local_nvram_mutex_lock(nvram);
    if (EZCFG_RET_OK != ret) {
      return EZCFG_RET_FAIL;
    }
    nvram_mutex_locked = 1;
  }

  for (i = 1; i < list_length+1; i++) {
    data = (struct ezcfg_nv_pair *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      if (nvram_mutex_locked) {
        _local_nvram_mutex_unlock(nvram);
      }
      return EZCFG_RET_FAIL;
    }
    if (data->n == NULL) {
      if (nvram_mutex_locked) {
        _local_nvram_mutex_unlock(nvram);
      }
      return EZCFG_RET_FAIL;
    }
    if (data->v) {
      free(data->v);
      data->v = NULL;
    }
    /* first try nvram */
    ret = _local_nvram_get_entry_value(nvram, data->n, &(data->v));
    if (EZCFG_RET_OK != ret) {
      /* then try meta_nvram */
      ret = local_meta_nvram_get_entry_value(ezcfg->meta_nvram, data->n, &(data->v));
      if (EZCFG_RET_OK != ret) {
        /* both nvram and meta_nvram don't have such value */
        if (nvram_mutex_locked) {
          _local_nvram_mutex_unlock(nvram);
        }
        return EZCFG_RET_FAIL;
      }
    }
  }

  if (nvram_mutex_locked) {
    _local_nvram_mutex_unlock(nvram);
  }
  return EZCFG_RET_OK;
}

int ezcfg_common_set_nvram_entries(struct ezcfg *ezcfg, struct ezcfg_linked_list *list)
{
  struct ezcfg_nv_pair *data = NULL;
  int i, list_length;
  int ret = EZCFG_RET_FAIL;
  int nvram_mutex_locked = 0;
  struct nvram *nvram = NULL;

  ASSERT (ezcfg != NULL);
  ASSERT (list != NULL);

  nvram = ezcfg->nvram;

  list_length = ezcfg_linked_list_get_length(list);

  if ((list_length > 0) && (nvram != NULL)) {
    ret = _local_nvram_mutex_lock(nvram);
    if (EZCFG_RET_OK != ret) {
      return EZCFG_RET_FAIL;
    }
    nvram_mutex_locked = 1;
  }

  for (i = 1; i < list_length+1; i++) {
    data = (struct ezcfg_nv_pair *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      if (nvram_mutex_locked) {
        _local_nvram_mutex_unlock(nvram);
      }
      return EZCFG_RET_FAIL;
    }
    if (data->n == NULL) {
      if (nvram_mutex_locked) {
        _local_nvram_mutex_unlock(nvram);
      }
      return EZCFG_RET_FAIL;
    }
    /* try nvram */
    if (data->v == NULL) {
      ret = _local_nvram_unset_entry(nvram, data->n);
    }
    else {
      ret = _local_nvram_set_entry(nvram, data->n, data->v);
    }
    if (EZCFG_RET_OK != ret) {
      /* both nvram and meta_nvram don't have such value */
      if (nvram_mutex_locked) {
        _local_nvram_mutex_unlock(nvram);
      }
      return EZCFG_RET_FAIL;
    }
  }

  if (nvram_mutex_locked) {
    _local_nvram_mutex_unlock(nvram);
  }
  return EZCFG_RET_OK;
}

int ezcfg_common_get_nvram_entries_by_ns(struct ezcfg *ezcfg, char *ns, struct ezcfg_linked_list **plist)
{
  struct ezcfg_linked_list *list = NULL;
  int ret = EZCFG_RET_FAIL;
  int nvram_mutex_locked = 0;
  struct nvram *nvram = NULL;

  ASSERT (ezcfg != NULL);
  ASSERT (ns != NULL);
  ASSERT (plist != NULL);

  list = ezcfg_linked_list_new(ezcfg,
    ezcfg_nv_pair_del_handler,
    ezcfg_nv_pair_cmp_handler);
  if (list == NULL) {
    return EZCFG_RET_FAIL;
  }

  nvram = ezcfg->nvram;

  if (nvram != NULL) {
    ret = _local_nvram_mutex_lock(nvram);
    if (EZCFG_RET_OK != ret) {
      goto exit_fail;
    }
    nvram_mutex_locked = 1;
  }

  /* first get from meta_nvram */
  ret = _local_meta_nvram_get_entries_by_ns(ezcfg->meta_nvram, ns, list);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }
  /* then get from nvram and replace the same name in meta_nvram */
  if (nvram != NULL) {
    ret = _local_nvram_get_entries_by_ns(nvram, ns, list);
    if (ret != EZCFG_RET_OK) {
      goto exit_fail;
    }
  }

  *plist = list;

  if (nvram_mutex_locked) {
    _local_nvram_mutex_unlock(nvram);
  }
  return EZCFG_RET_OK;

exit_fail:
  if (list)
    ezcfg_linked_list_del(list);

  if (nvram_mutex_locked) {
    _local_nvram_mutex_unlock(nvram);
  }
  return EZCFG_RET_FAIL;
}


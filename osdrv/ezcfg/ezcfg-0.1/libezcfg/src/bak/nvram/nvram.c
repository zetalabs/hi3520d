/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nvram/nvram.c
 *
 * Description  : implement Name-Value RAM
 * Warning      : must exclusively use it, say lock NVRAM before using it.
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-08-20   0.1       Write it from scratch
 * 2014-03-31   0.2       Rewrite it using sorted string storing approach
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

//static unsigned char default_magic[4] = {'N','V','R','M'};

//static unsigned char default_backend[4] = {'N','O','N','E'};

static unsigned char default_version[4] = {
  NVRAM_VERSOIN_MAJOR ,
  NVRAM_VERSOIN_MINOR ,
  NVRAM_VERSOIN_MICRO ,
  NVRAM_VERSOIN_REV ,
};

//static unsigned char default_coding[4] = {'N','O','N','E'};

struct nvram_node {
  char *name;
  char *value;
  size_t nlen; /* name length */
  size_t vlen; /* value length */
  struct ezcfg_socket *sock_list;
  int (*validate)(struct nvram_node *nvp);
  int (*propagate)(struct nvram_node *nvp);
};

/**************************************************************************
 *
 * NVRAM storage format
 *
 * The NVRAM is stored as a list of '\0' terminated "name=value"
 * strings. The end of the list is marked by a double '\0'.
 * New entries are added at "name" sorted position. Deleting an entry
 * shifts the remaining entries to the front. Replacing an entry is a
 * combination of deleting the old value and adding the new one.
 *
 * The NVRAM is preceeded by a header which contained info for
 * NVRAM version, coding scheme, storage medium and a 32 bit CRC over
 * the data part.
 *
 **************************************************************************
 */
struct ezcfg_nvram {
  struct ezcfg *ezcfg;

  pthread_mutex_t mutex; /* Protects nvram operations */

  struct nvram_node *node_list;
  size_t node_list_size;
  size_t node_list_used;

  size_t total_space_used;
};

/* mutex for increasing/decreasing ezcfg_nvram_ref_counter */
static pthread_mutex_t ezcfg_nvram_ref_mutex = PTHREAD_MUTEX_INITIALIZER;
static int ezcfg_nvram_ref_counter = 0;

/*********************/
/* Private functions */
/*********************/
static int lock_ezcfg_nvram_ref_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_lock(&(ezcfg_nvram_ref_mutex)) < 0) &&
         (retry < EZCFG_NVRAM_LOCK_RETRY_MAX)) {
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
  if (retry < EZCFG_NVRAM_LOCK_RETRY_MAX) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

static int unlock_ezcfg_nvram_ref_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_unlock(&(ezcfg_nvram_ref_mutex)) < 0) &&
         (retry < EZCFG_NVRAM_UNLOCK_RETRY_MAX)) {
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
  if (retry < EZCFG_NVRAM_UNLOCK_RETRY_MAX) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

static int nvram_node_clean(struct nvram_node *np)
{
  if (np->name != NULL) {
    free(np->name);
    np->name = NULL;
  }
  np->nlen = 0;

  if (np->value != NULL) {
    free(np->value);
    np->value = NULL;
  }
  np->vlen = 0;

  if (np->sock_list != NULL) {
    free(np->sock_list);
    np->sock_list = NULL;
  }

  return EZCFG_RET_OK;
}

static int find_nvram_entry_position(struct ezcfg_nvram *nvram, const char *name, bool *equal)
{
  int bottom, upper, middle;
  struct nvram_node *np;

  *equal = false;
  bottom = 0;
  upper = nvram->node_list_used;
  middle = (bottom + upper) / 2;

  while (bottom < upper) {
    middle = (bottom + upper) / 2;
    np = &(nvram->node_list[middle]);
    if (strcmp(np->name, name) == 0) {
      *equal = true;
      return middle;
    }
    else if (strcmp(np->name, name) < 0) {
      bottom = middle + 1;
    }
    else {
      upper = middle - 1;
    }
  }
  return middle;
}

static int nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
  struct ezcfg *ezcfg;
  char *tmp_name = NULL, *tmp_value = NULL;
  struct nvram_node *np;
  int i, j;
  bool equal;

  ezcfg = nvram->ezcfg;

  /* first entry */
  if (nvram->node_list_used == 0) {
    tmp_name = strdup(name);
    tmp_value = strdup(value);
    if ((tmp_name == NULL) || (tmp_value == NULL)) {
      goto fail_exit;
    }
    np = nvram->node_list;

    np->name = tmp_name;
    np->nlen = strlen(tmp_name) + 1;
    tmp_name = NULL;

    np->value = tmp_value;
    np->vlen = strlen(tmp_value) + 1;
    tmp_value = NULL;

    nvram->node_list_used += 1;

    nvram->total_space_used += np->nlen;
    nvram->total_space_used += np->vlen;

    return EZCFG_RET_OK;
  }

  i = find_nvram_entry_position(nvram, name, &equal);
  if (equal == true) {
    np = &(nvram->node_list[i]);
    if (strlen(value) < np->vlen) {
      sprintf(np->value, "%s", value);
      return EZCFG_RET_OK;
    }
    else {
      tmp_value = strdup(value);
      if (tmp_value == NULL) {
        return EZCFG_RET_FAIL;
      }
      free(np->value);
      np->value = tmp_value;
      nvram->total_space_used -= np->vlen;
      np->vlen = strlen(tmp_value) + 1;
      nvram->total_space_used += np->vlen;
      return EZCFG_RET_OK;
    }
  }
  else {
    /* not find nvram entry */
    if (nvram->node_list_used == nvram->node_list_size) {
      err(ezcfg, "no enough space for nvram entry set\n");
      return EZCFG_RET_FAIL;
    }
    /* insert nvram entry */
    tmp_name = strdup(name);
    tmp_value = strdup(value);
    if ((tmp_name == NULL) || (tmp_value == NULL)) {
      goto fail_exit;
    }

    for (j = nvram->node_list_used; j > i; j--) {
      nvram->node_list[j] = nvram->node_list[j-1];
    }

    np = &(nvram->node_list[i]);

    np->name = tmp_name;
    np->nlen = strlen(tmp_name) + 1;
    tmp_name = NULL;

    np->value = tmp_value;
    np->vlen = strlen(tmp_value) + 1;
    tmp_value = NULL;

    nvram->node_list_used += 1;

    nvram->total_space_used += np->nlen;
    nvram->total_space_used += np->vlen;

    return EZCFG_RET_OK;
  }

  fail_exit:
  if (tmp_name != NULL)
    free(tmp_name);
  if (tmp_value != NULL)
    free(tmp_value);
  return EZCFG_RET_FAIL;
}

static int nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name)
{
  struct nvram_node *np;
  int i, j;
  int ret;
  bool equal;

  //ezcfg = nvram->ezcfg;

  //name_len = strlen(name);

  i = find_nvram_entry_position(nvram, name, &equal);

  if (equal == true) {
    /* find nvram entry */
    np = &(nvram->node_list[i]);

    nvram->total_space_used -= np->nlen;
    nvram->total_space_used -= np->vlen;
    nvram->node_list_used -= 1;
    nvram_node_clean(np);

    for (j = i; j < (int)nvram->node_list_used; j++) {
      nvram->node_list[i] = nvram->node_list[i+1];
    }

    ret = EZCFG_RET_OK;
  }
  else {
    /* not find nvram entry */
    ret = EZCFG_RET_FAIL;
  }

  return ret;
}

static int nvram_commit(struct ezcfg_nvram *nvram)
{
  return EZCFG_RET_OK;
}

/* It's user's duty to free the returns string */
static int nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value)
{
  struct ezcfg *ezcfg;
  struct nvram_node *np;
  int i;
  bool equal;

  *value = NULL;
  ezcfg = nvram->ezcfg;

  /* find nvram entry position */
  i = find_nvram_entry_position(nvram, name, &equal);
  if (equal == true) {
    /* find nvram entry */
    np = &(nvram->node_list[i]);
    *value = strdup(np->value);
    if (*value == NULL) {
      err(ezcfg, "not enough memory for get nvram node.\n");
      return EZCFG_RET_FAIL;
    }
    return EZCFG_RET_OK;
  }
  return EZCFG_RET_FAIL;
}

static bool nvram_match_entry(struct ezcfg_nvram *nvram, const char *name1, char *name2)
{
  int p1, p2;
  bool equal1, equal2;
  struct nvram_node *np1, *np2;

  /* find nvram entry position */
  p1 = find_nvram_entry_position(nvram, name1, &equal1);

  if (equal1 == false) {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  np1 = &(nvram->node_list[p1]);

  /* find nvram entry position */
  p2 = find_nvram_entry_position(nvram, name2, &equal2);

  if (equal2 == false) {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  np2 = &(nvram->node_list[p2]);

  if (strcmp(np1->value, np2->value) == 0)
    return EZCFG_RET_OK;
  else
    return EZCFG_RET_FAIL;
}

static int nvram_match_entry_value(struct ezcfg_nvram *nvram, const char *name, char *value)
{
  int p;
  bool equal;
  struct nvram_node *np;

  /* find nvram entry position */
  p = find_nvram_entry_position(nvram, name, &equal);

  if (equal == false) {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  np = &(nvram->node_list[p]);

  if (strcmp(np->value, value) == 0)
    return EZCFG_RET_OK;
  else
    return EZCFG_RET_FAIL;
}

/*********************/
/* Public functions  */
/*********************/
int ezcfg_nvram_delete(struct ezcfg_nvram *nvram)
{
  struct nvram_node *np;

  ASSERT(nvram != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  if (nvram->node_list != NULL) {
    np = nvram->node_list;
    while (np->name != NULL) {
      nvram_node_clean(np);
      np++;
    }
    free(nvram->node_list);
    nvram->node_list = NULL;
    nvram->node_list_size = 0;
  }

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  pthread_mutex_destroy(&nvram->mutex);
  free(nvram);
  return EZCFG_RET_OK;
}

struct ezcfg_nvram *ezcfg_nvram_new(struct ezcfg *ezcfg, char *ns)
{
  struct ezcfg_nvram *nvram = NULL;
  char name[EZCFG_NAME_MAX];
  char val[EZCFG_VALUE_MAX];
  int ret;
  int node_list_size;
  struct nvram_node *node_list = NULL;

  ASSERT(ezcfg != NULL);

  /* increase ezcfg_ref_counter */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    return NULL;
  }

  /* get nvram node list size from meta nvram */
  ret = snprintf(name, sizeof(name), "%s%s", ns?ns:"", NVRAM_NAME(META, NVRAM_NODE_LIST_SIZE));
  if (ret >= sizeof(name)) {
    goto exit_fail;
  }

  ret = ezcfg_common_get_omni_nvram(ezcfg, name, val, sizeof(val));
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }

  node_list_size = atoi(val);
  if (node_list_size < 1) {
    goto exit_fail;
  }

  node_list = malloc(sizeof(struct nvram_node) * node_list_size);
  if (node_list == NULL) {
    EZDBG("can not malloc %d bytes for nvram node list.\n", node_list_size);
    err(ezcfg, "can not malloc %d bytes for nvram node list.\n", node_list_size);
    goto exit_fail;
  }

  nvram = calloc(1, sizeof(struct ezcfg_nvram));
  if (nvram == NULL) {
    err(ezcfg, "can not malloc nvram\n");
    EZDBG("can not malloc nvram\n");
    goto exit_fail;
  }

  pthread_mutex_init(&nvram->mutex, NULL);
  nvram->node_list_size = node_list_size;
  nvram->node_list = node_list;

  return nvram;

exit_fail:
  if (node_list) {
    free(node_list);
  }
  if (nvram) {
    nvram->node_list = NULL;
    ezcfg_nvram_delete(nvram);
  }
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("%s(%d) ezcfg_common_dec_ref() error.\n", __func__, __LINE__);
  }
  return NULL;
}

int ezcfg_nvram_get_version_string(struct ezcfg_nvram *nvram, char *buf, size_t len)
{
  ASSERT(nvram != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  snprintf(buf, len, "%d.%d.%d.%d", 
           default_version[0],
           default_version[1],
           default_version[2],
           default_version[3]);

  return EZCFG_RET_OK;
}

int ezcfg_nvram_set_entry(struct ezcfg_nvram *nvram, const char *name, const char *value)
{
  //struct ezcfg *ezcfg;
  int ret;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  //ezcfg = nvram->ezcfg;

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_set_entry(nvram, name, value);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

/* It's user's duty to free the returns string */
int ezcfg_nvram_get_entry_value(struct ezcfg_nvram *nvram, const char *name, char **value)
{
  int ret;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_get_entry_value(nvram, name, value);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_unset_entry(struct ezcfg_nvram *nvram, const char *name)
{
  int ret;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_unset_entry(nvram, name);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_commit(struct ezcfg_nvram *nvram)
{
  int ret;

  ASSERT(nvram != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_commit(nvram);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_initialize(struct ezcfg_nvram *nvram)
{
  return EZCFG_RET_OK;
}

int ezcfg_nvram_match_entry(struct ezcfg_nvram *nvram, char *name1, char *name2)
{
  int ret;

  ASSERT(nvram != NULL);
  ASSERT(name1 != NULL);
  ASSERT(name2 != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_match_entry(nvram, name1, name2);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);
  
  return ret;
}

int ezcfg_nvram_match_entry_value(struct ezcfg_nvram *nvram, char *name, char *value)
{
  int ret;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  ret = nvram_match_entry_value(nvram, name, value);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_is_valid_entry_value(struct ezcfg_nvram *nvram, char *name, char *value)
{
  //struct ezcfg *ezcfg;
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  //ezcfg = nvram->ezcfg;

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  //ret = ezcfg_nvram_validate_value(ezcfg, name, value);

  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_get_entries(struct ezcfg_nvram *nvram, struct ezcfg_linked_list *list)
{
  int ret = EZCFG_RET_FAIL;
  ezcfg_nv_pair_t *data;
  int i, list_length;

  ASSERT(nvram != NULL);
  ASSERT(list != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  /* parse settings */
  list_length = ezcfg_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (ezcfg_nv_pair_t *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      ret = EZCFG_RET_FAIL;
      goto func_out;
    }
    ret = nvram_get_entry_value(nvram, data->name, &(data->value));
    if (ret == EZCFG_RET_FAIL) {
      goto func_out;
    }
  }

 func_out:
  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_set_entries(struct ezcfg_nvram *nvram, struct ezcfg_linked_list *list)
{
  int ret = EZCFG_RET_FAIL;
  ezcfg_nv_pair_t *data;
  int i, list_length;

  ASSERT(nvram != NULL);
  ASSERT(list != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  /* parse settings */
  list_length = ezcfg_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (ezcfg_nv_pair_t *)ezcfg_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      ret = EZCFG_RET_FAIL;
      goto func_out;
    }
    ret = nvram_set_entry(nvram, data->name, data->value);
    if (ret == EZCFG_RET_FAIL) {
      goto func_out;
    }
  }

 func_out:
  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_set_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
  int ret = EZCFG_RET_FAIL;
  char *name, *value;
  int i, list_length;

  ASSERT(nvram != NULL);
  ASSERT(list != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  /* parse settings */
  list_length = ezcfg_link_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    name = ezcfg_link_list_get_node_name_by_index(list, i);
    value = ezcfg_link_list_get_node_value_by_index(list, i);
    if ((name == NULL) || (value == NULL)) {
      ret = EZCFG_RET_FAIL;
      goto func_out;
    }
    ret = nvram_set_entry(nvram, name, value);
    if (ret == EZCFG_RET_FAIL) {
      goto func_out;
    }
  }

 func_out:
  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

int ezcfg_nvram_unset_multi_entries(struct ezcfg_nvram *nvram, struct ezcfg_link_list *list)
{
  int ret = EZCFG_RET_FAIL;
  char *name;
  int i, list_length;

  ASSERT(nvram != NULL);
  ASSERT(list != NULL);

  /* lock nvram access */
  pthread_mutex_lock(&nvram->mutex);

  /* parse settings */
  list_length = ezcfg_link_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    name = ezcfg_link_list_get_node_name_by_index(list, i);
    if (name == NULL) {
      ret = EZCFG_RET_FAIL;
      goto func_out;
    }
    ret = nvram_unset_entry(nvram, name);
    if (ret == EZCFG_RET_FAIL) {
      goto func_out;
    }
  }

 func_out:
  /* unlock nvram access */
  pthread_mutex_unlock(&nvram->mutex);

  return ret;
}

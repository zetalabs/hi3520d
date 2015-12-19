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

#include "ezcfg_local.h"

struct nvram_node {
  char *name; /* null-terminated name string */
  char *value; /* null-terminated value string */
  int nlen; /* name length including null */
  int vlen; /* value length including null */
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
struct nvram {
  struct nvram_node *node_list;
  size_t node_list_size;
  size_t node_list_used;

  size_t total_space_used;
};


/* prototype for restricted functions */
int _local_nvram_mutex_lock(struct nvram *nvram);
int _local_nvram_mutex_unlock(struct nvram *nvram);
int _local_nvram_set_entry(struct nvram *nvram, const char *name, const char *value);
int _local_nvram_get_entry_value(struct nvram *nvram, const char *name, char **value);
int _local_nvram_unset_entry(struct nvram *nvram, const char *name);
int _local_nvram_get_node_list_size(struct nvram *nvram);
int _local_nvram_get_entries_by_ns(struct nvram *nvram, char *ns, struct ezcfg_linked_list *list);


/* mutex for maninpulate nvram */
static pthread_mutex_t nvram_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct nvram *this_nvram = NULL;

//static unsigned char default_magic[4] = {'N','V','R','M'};

//static unsigned char default_backend[4] = {'N','O','N','E'};

static unsigned char default_version[4] = {
  NVRAM_VERSOIN_MAJOR ,
  NVRAM_VERSOIN_MINOR ,
  NVRAM_VERSOIN_MICRO ,
  NVRAM_VERSOIN_REV ,
};

//static unsigned char default_coding[4] = {'N','O','N','E'};

/*********************/
/* Private functions */
/*********************/
static int lock_nvram_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_lock(&(nvram_mutex)) < 0) &&
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

static int unlock_nvram_mutex(void)
{
  int my_errno = 0;
  int retry = 0;
  while ((pthread_mutex_unlock(&(nvram_mutex)) < 0) &&
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

  return EZCFG_RET_OK;
}

static int find_nvram_entry_position(struct nvram *nvram, const char *name, bool *equal)
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

static int nvram_set_entry(struct nvram *nvram, const char *name, const char *value)
{
  char *tmp_name = NULL, *tmp_value = NULL;
  struct nvram_node *np;
  int i, j, len;
  bool equal;

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
    len = strlen(value);
    if (len < np->vlen) {
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
      EZDBG("no enough space for nvram entry set\n");
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

static int nvram_unset_entry(struct nvram *nvram, const char *name)
{
  struct nvram_node *np;
  int i, j;
  int ret;
  bool equal;

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

static int nvram_commit(struct nvram *nvram)
{
  return EZCFG_RET_OK;
}

/* It's user's duty to free the returned string */
static int nvram_get_entry_value(struct nvram *nvram, const char *name, char **value)
{
  struct nvram_node *np = NULL;
  int i = 0;
  bool equal = false;

  /* find nvram entry position */
  i = find_nvram_entry_position(nvram, name, &equal);
  if (equal == true) {
    /* find nvram entry */
    np = &(nvram->node_list[i]);
    *value = strdup(np->value);
    if (*value == NULL) {
      EZDBG("not enough memory for get nvram node.\n");
      return EZCFG_RET_FAIL;
    }
    return EZCFG_RET_OK;
  }
  return EZCFG_RET_FAIL;
}

static bool nvram_match_entry(struct nvram *nvram, const char *name1, char *name2)
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

static int nvram_match_entry_value(struct nvram *nvram, const char *name, char *value)
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
/* Restricted functions  */
/*********************/
int _local_nvram_mutex_lock(struct nvram *nvram)
{
  ASSERT(nvram != NULL);

  return lock_nvram_mutex();
}

int _local_nvram_mutex_unlock(struct nvram *nvram)
{
  ASSERT(nvram != NULL);

  return unlock_nvram_mutex();
}

int _local_nvram_set_entry(struct nvram *nvram, const char *name, const char *value)
{
  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  return nvram_set_entry(nvram, name, value);
}

int _local_nvram_get_entry_value(struct nvram *nvram, const char *name, char **value)
{
  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  return nvram_get_entry_value(nvram, name, value);
}

int _local_nvram_unset_entry(struct nvram *nvram, const char *name)
{
  ASSERT(nvram != NULL);
  ASSERT(name != NULL);

  return nvram_unset_entry(nvram, name);
}

int _local_nvram_get_node_list_size(struct nvram *nvram)
{
  ASSERT(nvram != NULL);
  return nvram->node_list_size;
}

int _local_nvram_get_entries_by_ns(struct nvram *nvram, char *ns, struct ezcfg_linked_list *list)
{
  struct ezcfg_nv_pair *data = NULL;
  int ret = EZCFG_RET_FAIL;
  struct nvram_node *np = NULL;
  int i = 0;
  int ns_len = 0;

  ASSERT(nvram != NULL);
  ASSERT(list != NULL);

  if (nvram->node_list == NULL) {
    return EZCFG_RET_OK;
  }

  if (ns)
    ns_len = strlen(ns) + 1;

  for (i = 0; i < (int)(nvram->node_list_size); i++) {
    np = &(nvram->node_list[i]);
    if (np->name == NULL) {
      return EZCFG_RET_OK;
    }

    if ((ns != NULL) &&
        ((np->nlen < ns_len) ||
         (strncmp(np->name, ns, ns_len) != 0))) {
      continue;
    }

    data = ezcfg_nv_pair_new(np->name, np->value);
    if (data == NULL) {
      return EZCFG_RET_FAIL;
    }
    ret = ezcfg_linked_list_append(list, data);
    if (ret != EZCFG_RET_OK) {
      ezcfg_nv_pair_del(data);
      return EZCFG_RET_FAIL;
    }
    ezcfg_nv_pair_del(data);
    data = NULL;
  }

  return EZCFG_RET_OK;
}

/*********************/
/* Public functions  */
/*********************/
int local_nvram_del(struct nvram *nvram)
{
  struct nvram_node *np = NULL;
  int i = 0;

  ASSERT(nvram != NULL);

  /* lock nvram_mutex */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  if (nvram->node_list != NULL) {
    for (i = 0; i < (int)(nvram->node_list_size); i++) {
      np = &(nvram->node_list[i]);
      if (np->name == NULL) {
        break;
      }
      nvram_node_clean(np);
    }
    free(nvram->node_list);
  }

  free(nvram);
  this_nvram = NULL;

  /* unlock nvram_mutex */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  return EZCFG_RET_OK;
}

struct nvram *local_nvram_new(char *buffer)
{
  struct nvram *nvram = NULL;
  char *val = NULL;
  int ret = EZCFG_RET_FAIL;
  int node_list_size = 0;
  struct nvram_node *node_list = NULL;

  /* get nvram node list size from meta nvram */
  ret = local_meta_nvram_get_entry_value(buffer, NVRAM_NAME(META, NVRAM_NODE_LIST_SIZE), &val);
  if (ret != EZCFG_RET_OK) {
    goto exit_fail;
  }

  node_list_size = atoi(val);
  free(val);
  val = NULL;
  if (node_list_size < 1) {
    goto exit_fail;
  }

  node_list = calloc(node_list_size, sizeof(struct nvram_node));
  if (node_list == NULL) {
    EZDBG("can not calloc %d bytes for nvram node list.\n",
          (int)(sizeof(struct nvram_node)*node_list_size));
    goto exit_fail;
  }

  nvram = calloc(1, sizeof(struct nvram));
  if (nvram == NULL) {
    EZDBG("can not malloc nvram\n");
    goto exit_fail;
  }

  /* lock nvram_mutex */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    goto exit_fail;
  }

  ASSERT (this_nvram == NULL);

  nvram->node_list_size = node_list_size;
  nvram->node_list = node_list;
  node_list = NULL;
  this_nvram = nvram;

  /* unlock nvram_mutex */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
    this_nvram = NULL;
    free(nvram->node_list);
    free(nvram);
    nvram = NULL;
  }
  return nvram;

exit_fail:
  if (node_list) {
    free(node_list);
  }
  if (nvram) {
    local_nvram_del(nvram);
  }
  return NULL;
}

int local_nvram_get_version_string(struct nvram *nvram, char *buf, size_t len)
{
  int n = 0;
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  /* lock nvram_mutex */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  n = snprintf(buf, len, "%d.%d.%d.%d", 
               default_version[0],
               default_version[1],
               default_version[2],
               default_version[3]);

  if ((n >= 0) && (n < (int)len)) {
    ret = EZCFG_RET_OK;
  }

  /* unlock nvram_mutex */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
    ret = EZCFG_RET_FAIL;
  }
  return ret;
}

int local_nvram_set_entry(struct nvram *nvram, const char *name, const char *value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_set_entry(nvram, name, value);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }

  return ret;
}

/* It's user's duty to free the returns string */
int local_nvram_get_entry_value(struct nvram *nvram, const char *name, char **value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_get_entry_value(nvram, name, value);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }

  return ret;
}

int local_nvram_unset_entry(struct nvram *nvram, const char *name)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_unset_entry(nvram, name);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }

  return ret;
}

int local_nvram_commit(struct nvram *nvram)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_commit(nvram);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }

  return ret;
}

int local_nvram_initialize(struct nvram *nvram)
{
  return EZCFG_RET_OK;
}

int local_nvram_match_entry(struct nvram *nvram, char *name1, char *name2)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name1 != NULL);
  ASSERT(name2 != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_match_entry(nvram, name1, name2);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }
  
  return ret;
}

int local_nvram_match_entry_value(struct nvram *nvram, char *name, char *value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(nvram != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  /* lock nvram access */
  if (lock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not lock nvram_mutex\n");
    return EZCFG_RET_FAIL;
  }

  ret = nvram_match_entry_value(nvram, name, value);

  /* unlock nvram access */
  if (unlock_nvram_mutex() != EZCFG_RET_OK) {
    EZDBG("can not unlock nvram_mutex\n");
  }

  return ret;
}

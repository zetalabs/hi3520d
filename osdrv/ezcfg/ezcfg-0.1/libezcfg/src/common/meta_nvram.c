/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/meta_nvram.c
 *
 * Description  : implement Name-Value RAM raw representation
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-27   0.1       Split it from nvram
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

/* prototype for restricted functions */
int _local_meta_nvram_set_entries(char *buffer, struct linked_list *list);
int _local_meta_nvram_get_entries_by_ns(char *buffer, char *ns, struct ezcfg_linked_list *list);

static unsigned char default_magic[4] = {'N','V','R','M'};

static unsigned char default_backend[4] = {'N','O','N','E'};

static unsigned char default_version[4] = {
  NVRAM_VERSOIN_MAJOR ,
  NVRAM_VERSOIN_MINOR ,
  NVRAM_VERSOIN_MICRO ,
  NVRAM_VERSOIN_REV ,
};

static unsigned char default_coding[4] = {'N','O','N','E'};

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
 * NVRAM storage medium, version, coding style and a 32 bit CRC over
 * the data part.
 *
 **************************************************************************
 */

/*
 * Private functions
 */

static char *find_nvram_entry_position(char *data, const char *name, bool *equal)
{
  int name_len, entry_len, cmp_len;
  char *entry;

  *equal = false;
  entry = data;
  name_len = strlen(name);

  while (*entry != '\0') {
    entry_len = strlen(entry) + 1;
    cmp_len = (entry_len > name_len) ? name_len : entry_len ;
    if (strncmp(name, entry, name_len) < 0) {
      break;
    }
    else if (strncmp(name, entry, name_len) == 0) {
      if (*(entry + cmp_len) == '=') {
        *equal = true;
      }
      break;
    }
    else {
      entry += entry_len;
    }
  }
  return entry;
}

static int meta_nvram_set_entry(char *buffer, const char *name, const char *value)
{
  int name_len, entry_len, new_entry_len;
  struct nvram_header *header;
  char *data, *p;
  bool equal;

  name_len = strlen(name);
  new_entry_len = name_len + strlen(value) + 2;

  header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);

  /* first entry */
  if (header->data_used == 0) {
    if (new_entry_len + 1 > (header->data_size - header->data_used)) {
      return EZCFG_RET_FAIL;
    }
    sprintf(data, "%s=%s", name, value);
    *(data + new_entry_len + 1) = '\0';
    header->data_used += (new_entry_len + 1);
    return EZCFG_RET_OK;
  }

  p = find_nvram_entry_position(data, name, &equal);
  if (equal == true) {
    /* find nvram entry */
    entry_len = strlen(p) + 1;
    if (new_entry_len > (header->data_size - header->data_used + entry_len)) {
      return EZCFG_RET_FAIL;
    }

    if (entry_len >= new_entry_len) {
      /* replace directory */
      p += name_len + 1;
      while (entry_len > new_entry_len) {
        *p = '=';
        p++;
        entry_len--;
      }
      sprintf(p, "%s", value);
      return EZCFG_RET_OK;
    }
    else {
      /* original entry is smaller than new entry, move backward */
      memmove(p + new_entry_len, p + entry_len, header->data_used - (p - data) - entry_len);
      p += name_len + 1;
      sprintf(p, "%s", value);
      header->data_used += (new_entry_len - entry_len);
      return EZCFG_RET_OK;
    }
  }
  else {
    /* not find nvram entry */
    if (new_entry_len > (header->data_size - header->data_used)) {
      return EZCFG_RET_FAIL;
    }
    /* insert nvram entry */
    memmove(p + new_entry_len, p, header->data_used - (p - data));
    sprintf(p, "%s=%s", name, value);
    header->data_used += new_entry_len;
    return EZCFG_RET_OK;
  }
}

static int meta_nvram_unset_entry(char *buffer, const char *name)
{
  struct nvram_header *header;
  char *data, *p;
  //int name_len;
  int entry_len;
  bool ret = false;
  bool equal;

  //name_len = strlen(name);

  header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);
  
  p = find_nvram_entry_position(data, name, &equal);

  if (equal == true) {
    /* find nvram entry */
    entry_len = strlen(p) + 1;
    memmove(p, p + entry_len, header->data_used - (p - data) - entry_len);
    header->data_used -= entry_len;
    ret = EZCFG_RET_OK;
  }
  else {
    /* not find nvram entry */
    ret = EZCFG_RET_FAIL;
  }

  return ret;
}

/* It's user's duty to free the returns string */
static int meta_nvram_get_entry_value(char *buffer, const char *name, char **value)
{
  //struct nvram_header *header;
  char *p, *data;
  int name_len;
  bool equal = false;

  //header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);
  name_len = strlen(name);
  *value = NULL;

  /* find nvram entry position */
  p = find_nvram_entry_position(data, name, &equal);

  if (equal == false) {
    return EZCFG_RET_FAIL;
  }

  /* find nvram entry */
  p += name_len;
  while (*p == '=') { p++; };
  *value = strdup(p);
  if (*value == NULL) {
    return EZCFG_RET_FAIL;
  }
  return EZCFG_RET_OK;
}

static int meta_nvram_match_entry(char *buffer, const char *name1, const char *name2)
{
  //struct nvram_header *header;
  char *data;
  char *p1, *p2;
  int name_len;
  bool equal1, equal2;

  //header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);

  /* find nvram entry position */
  name_len = strlen(name1);
  p1 = find_nvram_entry_position(data, name1, &equal1);

  if (equal1 == false) {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  p1 += name_len;
  while (*p1 == '=') { p1++; };

  /* find nvram entry position */
  name_len = strlen(name2);
  p2 = find_nvram_entry_position(data, name2, &equal2);

  if (equal2 == false) {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  p2 += name_len;
  while (*p2 == '=') { p2++; };

  if (strcmp(p1, p2) == 0)
    return EZCFG_RET_OK;
  else
    return EZCFG_RET_FAIL;
}

static int meta_nvram_match_entry_value(char *buffer, const char *name, char *value)
{
  //struct nvram_header *header;
  char *data;
  char *p;
  int name_len;
  bool equal;

  //header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);

  /* find nvram entry position */
  name_len = strlen(name);
  p = find_nvram_entry_position(data, name, &equal);

  if (*p == '\0') {
    return EZCFG_RET_FAIL;
  }
  /* find nvram entry */
  p += name_len;
  while (*p == '=') { p++; };

  if (strcmp(p, value) == 0)
    return EZCFG_RET_OK;
  else
    return EZCFG_RET_FAIL;
}

/*
 * Restricted functions
 */
int _local_meta_nvram_set_entries(char *buffer, struct linked_list *list)
{
  int ret = EZCFG_RET_FAIL;
  struct nv_pair *data;
  int i, list_length;

  ASSERT(buffer != NULL);
  ASSERT(list != NULL);

  /* parse settings */
  list_length = local_linked_list_get_length(list);
  for (i = 1; i < list_length+1; i++) {
    data = (struct nv_pair *)local_linked_list_get_node_data_by_index(list, i);
    if (data == NULL) {
      ret = EZCFG_RET_FAIL;
      goto func_out;
    }
    ret = meta_nvram_set_entry(buffer, data->name, data->value);
    if (ret == EZCFG_RET_FAIL) {
      goto func_out;
    }
  }

 func_out:
  return ret;
}

int _local_meta_nvram_get_entries_by_ns(char *buffer, char *ns, struct ezcfg_linked_list *list)
{
  struct ezcfg_nv_pair *data = NULL;
  int ret = EZCFG_RET_FAIL;
  char *p = NULL, *q = NULL;
  int ns_len = 0;
  char *name = NULL;
  char *value = NULL;
  int len = 0;

  ASSERT(buffer != NULL);
  ASSERT(list != NULL);

  if (ns)
    ns_len = strlen(ns);

  p = buffer + sizeof(struct nvram_header);
  while ((*p != '\0') || (*(p+1) != '\0')) {
    /* name */
    q = p;
    while ((*q != '=') && (*q != '\0')) q++;
    if (*q != '=') {
      return EZCFG_RET_FAIL;
    }
    len = q - p;

    if (ns != NULL) {
      if (len < ns_len)
        continue;
      if (strncmp(p, ns, ns_len) != 0)
        continue;
    }

    name = malloc(len + 1);
    if (name == NULL)
      goto exit_fail;
    if (len > 0)
      memcpy(name, p, len);
    name[len] = '\0';

    /* value */
    while ((*q == '=') && (*q != '\0')) q++;
    if (*q != '\0') p = q;
    while(*q != '\0') q++;
    len = q - p;
    value = malloc(len + 1);
    if (value == NULL)
      goto exit_fail;
    if (len > 0)
      memcpy(value, p, len);
    value[len] = '\0';

    data = ezcfg_nv_pair_new(name, value);
    if (data == NULL) {
      goto exit_fail;
    }
    free(name);
    name = NULL;
    free(value);
    value = NULL;
    
    ret = ezcfg_linked_list_append(list, data);
    if (ret != EZCFG_RET_OK) {
      goto exit_fail;
    }
    data = NULL;
  }

  return EZCFG_RET_OK;

exit_fail:
  if (name)
    free(name);

  if (value)
    free(value);

  if (data) {
    if (data->n)
      free(data->n);
    if (data->v)
      free(data->v);
  }

  return EZCFG_RET_FAIL;
}

/*
 * Public functions
 */
int local_meta_nvram_del(char *buffer)
{
  ASSERT(buffer != NULL);

  free(buffer);
  return true;
}

char *local_meta_nvram_new(int size)
{
  char *buffer;
  struct nvram_header *header;
  char *data;
  int i;

  /* must have the space for header and two '\0' */
  ASSERT(size >= (int)(sizeof(struct nvram_header) + 2));

  buffer = malloc(size);
  if (buffer == NULL) {
    return NULL;
  }

  memset(buffer, 0, size);

  header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);

  /* magic */
  for (i=0; i<4; i++) {
    header->magic[i] = default_magic[i];
  }
  /* version */
  for (i=0; i<4; i++) {
    header->version[i] = default_version[i];
  }
  /* coding */
  for (i=0; i<4; i++) {
    header->coding[i] = default_coding[i];
  }
  /* backend */
  for (i=0; i<4; i++) {
    header->backend[i] = default_backend[i];
  }

  header->data_size = size - sizeof(struct nvram_header);
  header->data_used = 0;

  header->data_crc = ezcfg_util_crc32((unsigned char *)data, header->data_size);
  header->header_crc = ezcfg_util_crc32((unsigned char *)header, sizeof(struct nvram_header) - sizeof(header->header_crc));

  return buffer;
}

int local_meta_nvram_set_version(char *buffer, char version[4])
{
  struct nvram_header *header;
  int i;

  ASSERT(buffer != NULL);

  header = (struct nvram_header *)buffer;

  /* version */
  for (i=0; i<4; i++) {
    header->version[i] = version[i];
  }

  return true;
}

int local_meta_nvram_set_coding_type(char *buffer, char coding[4])
{
  struct nvram_header *header;
  int i;

  ASSERT(buffer != NULL);

  header = (struct nvram_header *)buffer;

  /* coding */
  for (i=0; i<4; i++) {
    header->coding[i] = coding[i];
  }

  return true;
}

int local_meta_nvram_set_backend_type(char *buffer, char backend[4])
{
  struct nvram_header *header;
  int32_t i;

  ASSERT(buffer != NULL);

  header = (struct nvram_header *)buffer;

  /* backend */
  for (i=0; i<4; i++) {
    header->backend[i] = backend[i];
  }

  return true;
}

int local_meta_nvram_update_data_crc(char *buffer)
{
  struct nvram_header *header;
  char *data;

  ASSERT(buffer != NULL);

  header = (struct nvram_header *)buffer;
  data = buffer + sizeof(struct nvram_header);

  header->data_crc = ezcfg_util_crc32((unsigned char *)data, header->data_size);
  return EZCFG_RET_OK;
}

int local_meta_nvram_update_header_crc(char *buffer)
{
  struct nvram_header *header;

  ASSERT(buffer != NULL);

  header = (struct nvram_header *)buffer;

  header->header_crc = ezcfg_util_crc32((unsigned char *)header, sizeof(struct nvram_header) - sizeof(header->header_crc));
  return EZCFG_RET_OK;
}

int local_meta_nvram_set_entry(char *buffer, const char *name, const char *value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(buffer != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  ret = meta_nvram_set_entry(buffer, name, value);

  return ret;
}

int local_meta_nvram_unset_entry(char *buffer, const char *name)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(buffer != NULL);
  ASSERT(name != NULL);

  ret = meta_nvram_unset_entry(buffer, name);

  return ret;
}

/* It's user's duty to free the returns string */
int local_meta_nvram_get_entry_value(char *buffer, const char *name, char **value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(buffer != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  ret = meta_nvram_get_entry_value(buffer, name, value);

  return ret;
}

int local_meta_nvram_match_entry(char *buffer, const char *name1, const char *name2)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(buffer != NULL);
  ASSERT(name1 != NULL);
  ASSERT(name2!= NULL);

  ret = meta_nvram_match_entry(buffer, name1, name2);

  return ret;
}

int local_meta_nvram_match_entry_value(char *buffer, const char *name, char *value)
{
  int ret = EZCFG_RET_FAIL;

  ASSERT(buffer != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  ret = meta_nvram_match_entry_value(buffer, name, value);

  return ret;
}

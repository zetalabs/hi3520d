/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/nv_pair/nv_pair.c
 *
 * Description  : implement Name-Value pair operations
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-06   0.1       Split it from nvram
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

/*
 * Private functions
 */


/*
 * Public functions
 */

struct ezcfg_nv_pair *ezcfg_nv_pair_new(char *name, char *value)
{
  struct ezcfg_nv_pair *nvp = NULL;
  int len = 0;

  /* allocate nvp and clean it */
  nvp = calloc(1, sizeof(struct ezcfg_nv_pair));
  if (nvp == NULL) {
    goto exit_fail;
  }

  /* name */
  if (name) {
    len = strlen(name);
    nvp->n = malloc(len+1);
    if (nvp->n == NULL) {
      goto exit_fail;
    }
    nvp->nlen = len+1;
  }

  /* value */
  if (value) {
    len = strlen(value);
    nvp->v = malloc(len+1);
    if (nvp->v == NULL) {
      goto exit_fail;
    }
    nvp->vlen = len+1;
  }

  return nvp;

exit_fail:
  if (nvp) {
    if (nvp->n)
      free(nvp->n);
    if (nvp->v)
      free(nvp->v);
    free(nvp);
  }
  return NULL;
}

int ezcfg_nv_pair_del(struct ezcfg_nv_pair *nvp)
{
  ASSERT(nvp != NULL);

  if (nvp->n)
    free(nvp->n);

  if (nvp->v)
    free(nvp->v);

  free(nvp);

  return EZCFG_RET_OK;
}

int ezcfg_nv_pair_cmp_n(struct ezcfg_nv_pair *nvp1, struct ezcfg_nv_pair *nvp2)
{
  ASSERT(nvp1 != NULL);
  ASSERT(nvp2 != NULL);
  ASSERT(nvp1->n != NULL);
  ASSERT(nvp2->n != NULL);

  return strcmp(nvp1->n, nvp2->n);
}

int ezcfg_nv_pair_cmp_v(struct ezcfg_nv_pair *nvp1, struct ezcfg_nv_pair *nvp2)
{
  ASSERT(nvp1 != NULL);
  ASSERT(nvp2 != NULL);
  ASSERT(nvp1->v != NULL);
  ASSERT(nvp2->v != NULL);

  return strcmp(nvp1->v, nvp2->v);
}

char *ezcfg_nv_pair_get_n(struct ezcfg_nv_pair *nvp)
{
  ASSERT(nvp != NULL);

  return nvp->n;
}

char *ezcfg_nv_pair_get_v(struct ezcfg_nv_pair *nvp)
{
  ASSERT(nvp != NULL);

  return nvp->v;
}

int ezcfg_nv_pair_get_nlen(struct ezcfg_nv_pair *nvp)
{
  ASSERT(nvp != NULL);

  return nvp->nlen;
}

int ezcfg_nv_pair_get_vlen(struct ezcfg_nv_pair *nvp)
{
  ASSERT(nvp != NULL);

  return nvp->vlen;
}

int ezcfg_nv_pair_del_handler(void *data)
{
  ASSERT(data != NULL);

  return ezcfg_nv_pair_del((struct ezcfg_nv_pair *)data);
}

int ezcfg_nv_pair_cmp_handler(const void *d1, const void *d2)
{
  ASSERT(d1 != NULL);
  ASSERT(d2 != NULL);

  return ezcfg_nv_pair_cmp_n((struct ezcfg_nv_pair *)d1, (struct ezcfg_nv_pair *)d2);
}

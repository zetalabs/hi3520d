/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/nv_pair.c
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

#include "ezcfg_local.h"

/*
 * Private functions
 */


/*
 * Public functions
 */

int local_nv_pair_del_handler(void *data)
{
  struct nv_pair *nvp = NULL;

  ASSERT(data != NULL);

  nvp = (struct nv_pair *)data;

  if (nvp->name)
    free(nvp->name);

  if (nvp->value)
    free(nvp->value);

  free(nvp);

  return EZCFG_RET_OK;
}

int local_nv_pair_cmp_handler(const void *d1, const void *d2)
{
  struct nv_pair *nvp1 = NULL;
  struct nv_pair *nvp2 = NULL;

  ASSERT(d1 != NULL);
  ASSERT(d2 != NULL);

  nvp1 = (struct nv_pair *)d1;
  nvp2 = (struct nv_pair *)d2;

  ASSERT(nvp1->name != NULL);
  ASSERT(nvp2->name != NULL);

  return strcmp(nvp1->name, nvp2->name);
}


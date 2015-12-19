/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : common/nvpair.c
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

int ezcfg_nvpair_delete_handler(void *data)
{
  ezcfg_nv_pair_t *nvp = NULL;

  ASSERT(data != NULL);

  nvp = (ezcfg_nv_pair_t *)data;

  if (nvp->name)
    free(nvp->name);

  if (nvp->value)
    free(nvp->value);

  free(nvp);

  return EZCFG_RET_OK;
}

int ezcfg_nvpair_compare_handler(const void *d1, const void *d2)
{
  ezcfg_nv_pair_t *nvp1 = NULL;
  ezcfg_nv_pair_t *nvp2 = NULL;

  ASSERT(d1 != NULL);
  ASSERT(d2 != NULL);

  nvp1 = (ezcfg_nv_pair_t *)d1;
  nvp2 = (ezcfg_nv_pair_t *)d2;

  ASSERT(nvp1->name != NULL);
  ASSERT(nvp2->name != NULL);

  return strcmp(nvp1->name, nvp2->name);
}


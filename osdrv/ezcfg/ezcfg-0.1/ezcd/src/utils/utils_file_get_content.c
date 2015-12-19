/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_file_get_content.c
 *
 * Description  : get the file content
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Write it from scratch
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>

#include "ezcd.h"

int utils_file_get_content(const char *path, char **pbuf, size_t *plen)
{
  
  struct stat sb;
  FILE *fp = NULL;
  char *buf = NULL;

  if ((path == NULL) || (pbuf == NULL) || (plen == false)) {
    return EZCFG_RET_FAIL;
  }

  if (stat(path, &sb) == -1) {
    perror("stat");
    return EZCFG_RET_FAIL;
  }

  buf = malloc(sb.st_size + 1);
  if (buf == NULL) {
    perror("malloc");
    return EZCFG_RET_FAIL;
  }

  fp = fopen(path, "r");
  if (fp == NULL) {
    perror("fopen");
    goto fail_out;
  }

  *plen = fread(buf, sizeof(char), sb.st_size, fp);
  if (sb.st_size - *plen != 0) {
    perror("fread");
    goto fail_out;
  }
  /* All is OK! */
  fclose(fp);
  buf[sb.st_size] = '\0';
  *pbuf = buf;
  *plen += 1;
  return EZCFG_RET_OK;

fail_out:
  if (buf)
    free(buf);

  if (fp)
    fclose(fp);

  return EZCFG_RET_FAIL;
}

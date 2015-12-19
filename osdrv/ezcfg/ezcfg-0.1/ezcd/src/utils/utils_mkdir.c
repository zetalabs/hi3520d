/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : utils/utils_mkdir.c
 *
 * Description  : replace mkdir
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2012-03-06   0.1       Write it from scrach
 * ============================================================================
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

int utils_mkdir(const char *path, mode_t mode, bool is_dir)
{
  char buf[RC_COMMAND_LINE_SIZE];
  char *s;

  if (path == NULL)
    return -1;
  if (strlen(path) > (sizeof(buf) - 1))
    return -1;

  snprintf(buf, sizeof(buf), "%s", path);
  s = buf;
  while (*s != '\0') {
    /* skip first '/' */
    while (*s == '/') s++;
    /* find next first '/' */
    while((*s != '/') && (*s != '\0')) s++;
    if (*s == '/') {
      *s = '\0';
      if (mkdir(buf, mode) == -1) {
	if (errno == EEXIST) {
	  /* do nothing */
	}
	else {
	  return -1;
	}
      }
      *s = '/';
      s++;
    }
  }

  if (is_dir == true) {
    if (mkdir(path, mode) == -1) {
      if (errno == EEXIST) {
	return 0;
      }
      else {
	return -1;
      }
    }
  }
  return 0;
}

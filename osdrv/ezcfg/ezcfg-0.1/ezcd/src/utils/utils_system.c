/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * Module Name  : utils/utils_system.c
 *
 * Description  : execute command without shell, replace system() call
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-11-10   0.1       Write it from scrach
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

int utils_system(const char *command)
{
  char buf[RC_COMMAND_LINE_SIZE];
  char *argv[RC_MAX_ARGS];

  if (command == NULL)
    return -1;
  if (strlen(command) > (sizeof(buf) - 1))
    return -1;

  snprintf(buf, sizeof(buf), "%s", command);
  if (utils_parse_args(buf, strlen(buf) + 1, argv) < 1)
    return -1;

  return utils_execute(argv, NULL, NULL, 0, NULL);
}

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : main.c
 *
 * Description  : EZCD main program
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * 2014-05-13   0.2       Refine it for agent model
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
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <sys/un.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

int main(int argc, char **argv)
{
  char *name = strrchr(argv[0], '/');
  name = name ? name+1 : argv[0];

  if (!strcmp(argv[0], "/sbin/agent_env")) {
    return agent_env_main(argc, argv);
  }
  else if (!strcmp(name, "ezcd")) {
    return agent_env_main(argc, argv);
  }
  else if (!strcmp(name, "eznvc")) {
    return eznvc_main(argc, argv);
  }
  else {
    printf("Unknown name [%s]!\n", name);
    return (EXIT_FAILURE);
  }
}

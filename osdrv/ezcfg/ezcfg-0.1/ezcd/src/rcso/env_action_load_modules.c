/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_load_modules.c
 *
 * Description  : ezbox env agent run load kernel modules service
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2010-06-13   0.1       Write it from scratch
 * 2011-10-19   0.2       Modify it to use rcso framework
 * 2012-12-25   0.3       Modify it to use agent action framework
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
#include "pop_func.h"

#ifdef _EXEC_
int main(int argc, char **argv)
#else
  int env_action_load_modules(int argc, char **argv)
#endif
{
  FILE *file = NULL;
  char buf[32];
  int ret, flag, rc;

  if (argc < 2) {
    return (EXIT_FAILURE);
  }

  if (strcmp(argv[0], "load_modules")) {
    return (EXIT_FAILURE);
  }

  if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
    return (EXIT_FAILURE);
  }

  flag = utils_get_rc_act_type(argv[1]);

  /* first generate /etc/modules */
  pop_etc_modules(flag);

  file = fopen("/etc/modules", "r");
  if (file == NULL) {
    return (EXIT_FAILURE);
  }

  switch (flag) {
  case RC_ACT_BOOT :
  case RC_ACT_START :
    /* generate modules dependency */
    utils_system(CMD_DEPMOD);

    ret = EXIT_SUCCESS;
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      //ret = utils_install_kernel_module(buf, NULL);
      rc = utils_probe_kernel_module(buf, NULL);
      if (rc != EXIT_SUCCESS)
	ret = EXIT_FAILURE;
    }
    break;

  case RC_ACT_STOP :
    ret = EXIT_SUCCESS;
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      //ret = utils_remove_kernel_module(buf);
      rc = utils_clean_kernel_module(buf);
      if (rc != EXIT_SUCCESS)
	ret = EXIT_FAILURE;
    }
    break;

  default :
    ret = EXIT_FAILURE;
    break;
  }

  fclose(file);
  return (ret);
}

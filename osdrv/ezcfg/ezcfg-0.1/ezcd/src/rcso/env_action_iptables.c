/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_iptables.c
 *
 * Description  : env agent manipulates firewall and port-mapping
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-05-17   0.1       Write it from scratch
 * 2011-10-22   0.2       Modify it to use rcso framework
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

#if 0
#define DBG(format, args...) do {				\
    char dbg_path[256];						\
    FILE *dbg_fp;						\
    snprintf(dbg_path, 256, "/tmp/%d-debug.txt", getpid());	\
    dbg_fp = fopen(dbg_path, "a");				\
    if (dbg_fp) {						\
      fprintf(dbg_fp, format, ## args);				\
      fclose(dbg_fp);						\
    }								\
  } while(0)
#else
#define DBG(format, args...)
#endif

#if (HAVE_EZBOX_LAN_NIC == 1)
static int lan_iptables(int flag)
{
  int rc;
  rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
  if (rc < 0) {
    return (EXIT_FAILURE);
  }

  switch (flag) {
  case RC_ACT_RESTART :
  case RC_ACT_STOP :
    pop_etc_l7_protocols(RC_ACT_STOP);
    if (flag == RC_ACT_STOP) {
      break;
    }

    /* RC_ACT_RESTART fall through */
  case RC_ACT_START :
    pop_etc_l7_protocols(RC_ACT_START);
    break;

  default :
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
static int wan_iptables(int flag)
{
  int rc;
  rc = utils_nvram_cmp(NVRAM_SERVICE_OPTION(RC, IPTABLES_ENABLE), "1");
  if (rc < 0) {
    return (EXIT_FAILURE);
  }

  switch (flag) {
  case RC_ACT_RESTART :
  case RC_ACT_STOP :
    pop_etc_l7_protocols(RC_ACT_STOP);
    if (flag == RC_ACT_STOP) {
      break;
    }

    /* RC_ACT_RESTART fall through */
  case RC_ACT_START :
    pop_etc_l7_protocols(RC_ACT_START);
    break;

  default :
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}
#endif

static int load_iptables_modules(int flag)
{
  int ret, fail_flag = 0;
  FILE *file = NULL;
  char buf[32];

  switch (flag) {
  case RC_ACT_BOOT :
    /* manage iptables configuration options */
    mkdir("/etc/l7-protocols", 0755);
    mkdir("/etc/iptables", 0755);
    ret = EXIT_SUCCESS;
    break;

  case RC_ACT_RESTART :
  case RC_ACT_STOP :
    /* first generate /etc/iptables/modules */
    pop_etc_iptables_modules(RC_ACT_STOP);

    file = fopen("/etc/iptables/modules", "r");
    if (file == NULL) {
      return (EXIT_FAILURE);
    }

    /* remove iptables kernel modules */
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      ret = utils_remove_kernel_module(buf);
    }

    if (flag == RC_ACT_STOP) {
      ret = EXIT_SUCCESS;
      break;
    }

    /* RC_ACT_RESTART fall through */
  case RC_ACT_START :
    /* first generate /etc/iptables/modules */
    pop_etc_iptables_modules(RC_ACT_START);

    file = fopen("/etc/iptables/modules", "r");
    if (file == NULL) {
      return (EXIT_FAILURE);
    }

    /* install iptables kernel modules */
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      //ret = utils_install_kernel_module(buf, NULL);
      ret = utils_probe_kernel_module(buf, NULL);
      if (ret != EXIT_SUCCESS) {
	fail_flag = 1;
      }
    }

    if (fail_flag == 0)
      ret = EXIT_SUCCESS;
    break;

  default :
    ret = EXIT_FAILURE;
    break;
  }

  if (file != NULL)
    fclose(file);

  return ret;
}

static int iptables_firewall(int flag)
{
  FILE *file;
  int ret;
  char buf[256];
  bool rc;

  switch (flag) {
  case RC_ACT_RESTART :
  case RC_ACT_STOP :
    /* first remove /etc/iptables/firewall */
    pop_etc_iptables_firewall(RC_ACT_STOP);

    /* stop firewall */
    snprintf(buf, sizeof(buf), "%s -t mangle -F", CMD_IPTABLES);
    utils_system(buf);
    snprintf(buf, sizeof(buf), "%s -t nat -F", CMD_IPTABLES);
    utils_system(buf);
    snprintf(buf, sizeof(buf), "%s -t filter -F", CMD_IPTABLES);
    utils_system(buf);

    /* FIXME: also clean conntrack */

    if (flag == RC_ACT_STOP) {
      ret = EXIT_SUCCESS;
      break;
    }

    /* RC_ACT_RESTART fall through */
  case RC_ACT_START :
    /* first generate /etc/iptables/firewall */
    pop_etc_iptables_firewall(RC_ACT_START);

    /* then enable IP forward */
    file = fopen("/proc/sys/net/ipv4/ip_forward", "w");
    if (file == NULL) {
      ret = EXIT_FAILURE;
      break;
    }
    rc = utils_file_print_line(file, buf, sizeof(buf), "%d", 1);
    fclose(file);
    if (rc == false) {
      ret = EXIT_FAILURE;
      break;
    }

    /* finally restore the firewall rules */
    snprintf(buf, sizeof(buf), "%s %s", CMD_IPTABLES_RESTORE, "/etc/iptables/firewall");
    utils_system(buf);

    ret = EXIT_SUCCESS;
    break;

  default :
    ret = EXIT_FAILURE;
    break;
  }

  return ret;
}

#ifdef _EXEC_
int main(int argc, char **argv)
#else
  int env_action_iptables(int argc, char **argv)
#endif
{
  int flag;

  if (argc < 3) {
    return (EXIT_FAILURE);
  }

  if (strcmp(argv[0], "iptables")) {
    return (EXIT_FAILURE);
  }

  if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
    return (EXIT_FAILURE);
  }

  flag = utils_get_rc_act_type(argv[2]);

  if (strcmp(argv[1], "load") == 0) {
    return load_iptables_modules(flag);
  }
  else if (strcmp(argv[1], "firewall") == 0) {
    return iptables_firewall(flag);
  }
#if (HAVE_EZBOX_LAN_NIC == 1)
  else if (strcmp(argv[1], "lan") == 0) {
    return lan_iptables(flag);
  }
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
  else if (strcmp(argv[1], "wan") == 0) {
    return wan_iptables(flag);
  }
#endif
  else {
    return (EXIT_FAILURE);
  }
}

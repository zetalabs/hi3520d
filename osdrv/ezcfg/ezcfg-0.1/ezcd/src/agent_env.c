/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : agent_env.c
 *
 * Description  : ezbox agent env
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-07-18   0.1       Write it from scratch
 * 2015-06-14   0.2       Reimplement it by using socket_agent object
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
#include <dlfcn.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/resource.h>
#include <stdarg.h>

#include "ezcd.h"

#define handle_error_en(en, msg) \
  do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#if 1
#define DBG(format, args...)                      \
  do {                                            \
    FILE *dbg_fp;                                 \
    if (debug == true)                            \
      dbg_fp = fopen("/tmp/kmsg", "a");           \
    else                                          \
      dbg_fp = fopen("/dev/kmsg", "a");	          \
    if (dbg_fp) {                                 \
      fprintf(dbg_fp, format, ## args);           \
      fclose(dbg_fp);                             \
    }                                             \
  } while(0)
#else
#define DBG(format, args...)
#endif

#define INFO(format, args...)                     \
  do {                                            \
    FILE *info_fp;                                \
    if (debug == true)                            \
      info_fp = fopen("/tmp/kmsg", "a");          \
    else                                          \
      info_fp = fopen("/dev/kmsg", "a");          \
    if (info_fp) {                                \
      fprintf(info_fp, format, ## args);          \
      fclose(info_fp);                            \
    }                                             \
  } while(0)


#define AGENT_ENV_PRIORITY	-4

#define AGENT_ENV_CONFIG_FILE_PATH	"/etc/agent/env/default.conf"

#ifndef RB_HALT_SYSTEM
#  define RB_HALT_SYSTEM  0xcdef0123
#  define RB_POWER_OFF    0x4321fedc
#  define RB_AUTOBOOT     0x01234567
#endif

static void agent_env_show_usage(char *name)
{
  printf("Usage: %s [-c config file]\n", name);
  printf("          [-i init config]\n");
  printf("          [-n namespace]\n");
  printf("\n");
  printf("  [-c]--\n");
  printf("    config file, default : \"%s\n", AGENT_ENV_CONFIG_FILE_PATH);
  printf("  [-i]--\n");
  printf("    init config by NVRAM JSON representation, ex: \"{\"name\":\"value\"}\"\n");
  printf("  [-n]--\n");
  printf("    namespace\n");
  printf("\n");
}


int agent_env_main(int argc, char **argv)
{
  bool debug = false;
  int opt = 0;
  int rc = 0;
  char *ns = NULL;
  char *conf_file = AGENT_ENV_CONFIG_FILE_PATH;
  char *init_conf = NULL;
  size_t init_conf_len = 0;
  int s = 0;
  char *name = strrchr(argv[0], '/');
  name = name ? name+1 : argv[0];

  if (!strcmp(name, "ezcd")) {
    debug = true;
  }

  while ((opt = getopt(argc, argv, "c:i:n:")) != -1) {
    switch (opt) {
    case 'c':
      conf_file = optarg;
      break;
    case 'i':
      if (init_conf != NULL) {
        printf("init_conf has been set to [%s]\n", init_conf);
        printf("skip setting to [%s]\n", optarg);
      }
      else {
        init_conf = strdup(optarg);
      }
      break;
    case 'n':
      ns = optarg;
      break;
    default: /* '?' */
      agent_env_show_usage(argv[0]);
      rc = -EZCFG_E_ARGUMENT;
      goto func_out;
    }
  }


  /* try to get init_conf from conf_file */
  if (init_conf == NULL) {
    if (EZCFG_RET_OK != utils_file_get_content(conf_file, &init_conf, &init_conf_len)) {
      printf("can't get file [%s] content.\n", conf_file);
      rc = -EZCFG_E_ARGUMENT;
      goto func_out;
    }
  }

  /* unset umask */
  s = chdir("/");
  if (s == -1) {
    DBG("<6>agent_env: chdir error!\n");
    rc = -EZCFG_E_RESOURCE;
    goto func_out;
  }
  umask(0);

  /* make the command line just say "agent_env"  - thats all, nothing else */
  strncpy(argv[0], "agent_env", strlen(argv[0]));
  /* wipe argv[1]-argv[N] so they don't clutter the ps listing */
  while (*++argv)
    memset(*argv, 0, strlen(*argv));

  rc = ezcfg_api_agent_start(init_conf, ns);

  /* should never run to this place!!! */
func_out:
  DBG("<6>agent_env: should never run to this place!!!\n");
  if (init_conf)
    free(init_conf);

  return rc;
}

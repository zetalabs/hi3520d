/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : eznvc.c
 *
 * Description  : ezbox config NVRAM change client
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
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcd.h"

static void eznvc_show_usage(void)
{
  printf("Usage: eznvc [-q] [-c config file]\n");
  printf("             [-j \"NVRAM JSON representation\"]\n");
  printf("             [-f NVRAM JSON representation file]\n");
  printf("             [-n namespace]\n");
  printf("\n");
  printf("  [-q]--\n");
  printf("    run in quiet mode\n");
  printf("  [-c]--\n");
  printf("    config file, default : \"%s\n", EZNVC_CONFIG_FILE_PATH);
  printf("  [-j]--\n");
  printf("    NVRAM JSON representation, ex: \"{\"name\":\"value\"}\"\n");
  printf("  [-f]--\n");
  printf("    NVRAM JSON representation file\n");
  printf("  [-n]--\n");
  printf("    namespace\n");
  printf("\n");
}

int eznvc_main(int argc, char **argv)
{
  int opt = 0;
  int rc = 0;
  bool quiet_mode = false;
  char *ns = NULL;
  char *conf_file = EZNVC_CONFIG_FILE_PATH;
  char *nv_json = NULL;
  size_t nv_json_len = 0;
  char *init_conf = NULL;
  char *result = NULL;
  size_t init_conf_len = 0;

  while ((opt = getopt(argc, argv, "qc:j:f:n:")) != -1) {
    switch (opt) {
    case 'q':
      quiet_mode = true;
      break;
    case 'c':
      conf_file = optarg;
      break;
    case 'n':
      ns = optarg;
      break;
    case 'j':
      if (nv_json != NULL) {
        printf("NVRAM JSON has been set to [%s]\n", nv_json);
        printf("skip setting to [%s]\n", optarg);
      }
      else {
        nv_json = strdup(optarg);
        nv_json_len = strlen(optarg);
      }
      break;
    case 'f':
      if (nv_json != NULL) {
        printf("NVRAM JSON has been set to [%s]\n", nv_json);
        printf("skip reading from file [%s]\n", optarg);
      }
      else {
        if (EZCFG_RET_OK != utils_file_get_content(optarg, &nv_json, &nv_json_len)) {
          printf("can't get file [%s] content.\n", optarg);
        }
      }
      break;
    default: /* '?' */
      eznvc_show_usage();
      rc = -EZCFG_E_ARGUMENT;
      goto func_out;
    }
  }

  if (nv_json == NULL) {
    printf("NVRAM JSON does not set!\n");
    rc = -EZCFG_E_ARGUMENT;
    goto func_out;
  }

  if (conf_file == NULL) {
    printf("config file does not set!\n");
    rc = -EZCFG_E_ARGUMENT;
    goto func_out;
  }

  if (EZCFG_RET_OK != utils_file_get_content(conf_file, &init_conf, &init_conf_len)) {
    printf("can't get file [%s] content.\n", conf_file);
    rc = -EZCFG_E_ARGUMENT;
    goto func_out;
  }

  rc = ezcfg_api_nvram_change(init_conf, ns, nv_json, &result);
  if (quiet_mode == false) {
    if (rc < 0) {
      printf("ERROR\n");
    }
    else {
      printf("OK\n");
    }
  }
func_out:
  if (init_conf)
    free(init_conf);

  if (nv_json)
    free(nv_json);

  if (result)
    free(result);

  return rc;
}

/* ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : util/util_snprintf_ns_name.c
 *
 * Description  : combine namespace with name
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-08   0.1       Write it from scratch
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
#include <assert.h>
#include <pthread.h>
#include <errno.h>
#include <syslog.h>
#include <ctype.h>
#include <stdarg.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

int ezcfg_util_snprintf_ns_name(char *buf, size_t size, char *ns, char *name)
{
  int n = -1;

  if ((buf == NULL) || (size < 1) || (name == NULL)) {
    return EZCFG_RET_FAIL;
  }

  n = snprintf(buf, size, "%s%s", ns?ns:"", name);
  if ((n < 0) || (n >= (int)size)) {
    return EZCFG_RET_FAIL;
  }
  else {
    return EZCFG_RET_OK;
  }
}

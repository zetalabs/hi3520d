/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : env_action_preboot.c
 *
 * Description  : ezbox env agent prepare to boot system
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-28   0.1       Write it from scratch
 * 2011-10-16   0.2       Modify it to use rcso framework
 * 2012-12-21   0.3       Modify it to use agent action framework
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
#include "utils.h"

#if 0
#define DBG(format, args...) do {		\
    FILE *dbg_fp = fopen("/dev/kmsg", "a");	\
    if (dbg_fp) {				\
      fprintf(dbg_fp, format, ## args);		\
      fclose(dbg_fp);				\
    }						\
  } while(0)
#else
#define DBG(format, args...)
#endif

#define DBG2() do {					\
    pid_t pid = getpid();				\
    FILE *fp = fopen("/dev/kmsg", "a");			\
    if (fp) {						\
      char buf[32];					\
      FILE *fp2;					\
      int i;						\
      for(i=pid; i<pid+30; i++) {			\
	snprintf(buf, sizeof(buf), "/proc/%d/stat", i); \
	fp2 = fopen(buf, "r");				\
	if (fp2) {					\
	  if (fgets(buf, sizeof(buf)-1, fp2) != NULL) { \
	    fprintf(fp, "pid=[%d] buf=%s\n", i, buf);	\
	  }						\
	  fclose(fp2);					\
	}						\
      }							\
      fclose(fp);					\
    }							\
  } while(0)

#ifdef _EXEC_
int main(int argc, char **argv)
#else
  int env_action_preboot(int argc, char **argv)
#endif
{
  char buf[KERNEL_COMMAND_LINE_SIZE];
  int ret;
  char *p, *q;
  struct stat stat_buf;
  int flag = RC_ACT_UNKNOWN;

  if (argc < 2) {
    return (EXIT_FAILURE);
  }

  if (strcmp(argv[0], "preboot")) {
    return (EXIT_FAILURE);
  }

  if (utils_init_ezcfg_api(EZCD_CONFIG_FILE_PATH) == false) {
    return (EXIT_FAILURE);
  }

  flag = utils_get_rc_act_type(argv[1]);

  switch (flag) {
  case RC_ACT_BOOT :
    /* setup basic directory structure */
    utils_make_preboot_dirs();

    DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());

    /* run in root HOME path */
    setenv("HOME", ROOT_HOME_PATH, 1);
    ret = chdir(ROOT_HOME_PATH);
    if (ret == -1) {
      DBG("huedebug %s(%d) pid=[%d] chdir(%s) fail!\n", __func__, __LINE__, getpid(), ROOT_HOME_PATH);
    }

    /* get the kernel module name from /proc/cmdline */
    ret = utils_get_kernel_modules(buf, sizeof(buf));
    if (ret > 0) {

      /* generate modules dependency */
      utils_system(CMD_DEPMOD);

      /* load preboot kernel modules */
      p = buf;
      while(p != NULL) {
	q = strchr(p, ',');
	if (q != NULL)
	  *q = '\0';

	//utils_install_kernel_module(p, NULL);
	utils_probe_kernel_module(p, NULL);

	if (q != NULL)
	  p = q+1;
	else
	  p = NULL;
      }
    }

    /* init /dev/ nodes */
    utils_udev_pop_nodes();

    /* get the kernel "init=" from /proc/cmdline */
    ret = utils_get_kernel_init(buf, sizeof(buf));
    /* check if we need switch root device */
    if ((ret > 0) && (utils_switch_root_is_ready(buf) == true)) {
      DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
      utils_clean_preboot_dirs();
      /* if switch_root succeed, it should never return */
      utils_switch_root_device(buf);
      /* switch_root fail, fall through */
      DBG("huedebug %s(%d) pid=[%d]\n", __func__, __LINE__, getpid());
      exit(EXIT_FAILURE);
    }

    /* prepare boot device path */
    utils_mount_boot_partition_writable();

    /* check if we need umount /boot so that */
    /* we may have a chance to copy the ezbox_boot.cfg.dft to ezbox_boot.cfg */
    if (utils_boot_partition_is_ready() == false) {
      utils_umount_boot_partition();
    }

    /* check if the ezbox_boot.cfg is ready */
    if ((stat(BOOT_CONFIG_FILE_PATH, &stat_buf) != 0) ||
	(!S_ISREG(stat_buf.st_mode))) {
      /* try to restore to default ezbox_boot.cfg */
      if ((stat(BOOT_CONFIG_DEFAULT_FILE_PATH, &stat_buf) == 0) &&
	  (S_ISREG(stat_buf.st_mode))) {
	/* remove ezbox_boot.cfg */
	utils_system("rm -rf " BOOT_CONFIG_FILE_PATH);
	/* copy default ezbox_boot.cfg.dft ezbox_boot.cfg */
	utils_system("cp -f " BOOT_CONFIG_DEFAULT_FILE_PATH " " BOOT_CONFIG_FILE_PATH);
      }
    }

    if (utils_boot_partition_is_ready() == true) {
      /* make /boot read-only */
      utils_remount_boot_partition_readonly();
    }

    ret = EXIT_SUCCESS;
    break;

  default:
    ret = EXIT_FAILURE;
    break;
  }

  return (ret);
}

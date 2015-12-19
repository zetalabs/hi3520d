/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_pop_dev_nodes.c
 *
 * Description  : ezbox populate /dev/ nodes
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-10-16   0.1       Write it from scratch
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
#include <dirent.h>

#include "ezcd.h"

#ifdef PATH_MAX
#undef PATH_MAX
#endif
#define PATH_MAX        512

#define SYS_BASE        "/sys"
#define BLOCK_BASE      "/sys/block"
#define CLASS_BASE      "/sys/class"
#define UEVENT_BASE     "/uevent"
#define DEVICES_BASE    "/devices"
#define DEV_BASE        "/dev"

#define OP_TRIGGER      0x0001
#define OP_POP_NODE     0x0002

static int verbose = 1;

static void trigger_uevent(const char *devpath)
{
  char filename[PATH_MAX];
  int fd;

  //snprintf(filename, sizeof(filename), "/sys%s/uevent", devpath);
  snprintf(filename, sizeof(filename), "%s%s%s", SYS_BASE, devpath, UEVENT_BASE);

  if (verbose)
    EZDBG("trigger %s\n", devpath);

  fd = open(filename, O_WRONLY);
  if (fd < 0) {
    EZDBG("error on opening %s: %s\n", filename, strerror(errno));
    return;
  }

  if (write(fd, "add", 3) < 0)
    EZDBG("error on triggering %s: %s\n", filename, strerror(errno));

  close(fd);
}

static void make_device(const char *devpath)
{
  char filename[PATH_MAX];
  char buf[16];
  int major, minor, type, mode;
  char *node_name;
  int fd;

  //snprintf(filename, sizeof(filename), "/sys%s/dev", devpath);
  snprintf(filename, sizeof(filename), "%s%s%s", SYS_BASE, devpath, DEV_BASE);

  if (verbose)
    EZDBG("make_device %s\n", devpath);

  fd = open(filename, O_RDONLY);
  if (fd < 0) {
    EZDBG("error on opening %s: %s\n", filename, strerror(errno));
    return;
  }

  if (read(fd, buf, sizeof(buf)) < 1) {
    EZDBG("error on read %s: %s\n", filename, strerror(errno));
    goto func_out;
  }

  if (sscanf(buf, "%u:%u", &major, &minor) != 2) {
    EZDBG("error on file format : %s\n", filename);
    goto func_out;
  }

  type = S_IFCHR;
  if (strstr(devpath, "/block/"))
    type = S_IFBLK;

  mode = 0500;
  node_name = strrchr(devpath, '/');
  if (node_name == NULL) {
    EZDBG("devpath error : %s\n", devpath);
    goto func_out;
  }

  snprintf(filename, sizeof(filename), "%s/%s", DEV_BASE, node_name);
  node_name = filename;
  if (major >= 0) {
    if (mknod(node_name, mode | type, makedev(major, minor)) && errno != EEXIST) {
      EZDBG("can't create '%s'\n", node_name);
    }
  }

func_out:
  close(fd);
}

static int sysfs_resolve_link(char *devpath, size_t size)
{
  char link_path[PATH_MAX];
  char link_target[PATH_MAX];
  int len;
  int i;
  int back;

  //snprintf(link_path, sizeof(link_path), "/sys%s", devpath);
  snprintf(link_path, sizeof(link_path), "%s%s", SYS_BASE, devpath);
  len = readlink(link_path, link_target, sizeof(link_target));
  if (len <= 0)
    return -1;
  link_target[len] = '\0';
  EZDBG("path link '%s' points to '%s'\n", devpath, link_target);

  for (back = 0; strncmp(&link_target[back * 3], "../", 3) == 0; back++)
		;
  EZDBG("base '%s', tail '%s', back %i\n", devpath, &link_target[back * 3], back);
  for (i = 0; i <= back; i++) {
    char *pos = strrchr(devpath, '/');

    if (pos == NULL)
      return -1;
    pos[0] = '\0';
  }
  EZDBG("after moving back '%s'\n", devpath);
  len = strlen(devpath);
  snprintf(devpath+len, size - len, "/%s", &link_target[back * 3]);
  EZDBG("devpath='%s'\n", devpath);
  return 0;
}

static int device_list_insert_op(const char *path, int op)
{
  char filename[PATH_MAX];
  char devpath[PATH_MAX];
  struct stat statbuf;

  EZDBG("op=%d '%s'\n" , op, path);

  if (op == OP_TRIGGER) {
    /* we only have a device, if we have an uevent file */
    //snprintf(filename, sizeof(filename), "%s/uevent", path);
    snprintf(filename, sizeof(filename), "%s%s", path, UEVENT_BASE);
    EZDBG("filename='%s'\n" , filename);
    if (stat(filename, &statbuf) < 0) {
      return -1;
    }
    if (!(statbuf.st_mode & S_IWUSR)) {
      return -1;
    }
  }
  else if (op == OP_POP_NODE) {
    /* we only have a device, if we have an dev file */
    //snprintf(filename, sizeof(filename), "%s/dev", path);
    snprintf(filename, sizeof(filename), "%s%s", path, DEV_BASE);
    EZDBG("filename='%s'\n" , filename);
    if (stat(filename, &statbuf) < 0) {
      return -1;
    }
    if (!(statbuf.st_mode & S_IRUSR)) {
      return -1;
    }
  }
  else {
    EZDBG("unknown op!\n");
    return -1;
  }

  snprintf(devpath, sizeof(devpath), "%s", &path[4]);

  /* resolve possible link to real target */
  if (lstat(path, &statbuf) < 0) {
    return -1;
  }
  if (S_ISLNK(statbuf.st_mode))
    if (sysfs_resolve_link(devpath, sizeof(devpath)) != 0)
      return -1;

  if (op == OP_TRIGGER) {
    trigger_uevent(devpath);
  }
  else if (op == OP_POP_NODE) {
    make_device(devpath);
  }

  return 0;
}

static void scan_subsystem(const char *subsys, int op)
{
  char base[PATH_MAX];
  DIR *dir;
  struct dirent *dent;

  //snprintf(base, sizeof(base), "/sys/%s", subsys);
  snprintf(base, sizeof(base), "%s/%s", SYS_BASE, subsys);

  dir = opendir(base);
  if (dir != NULL) {
    for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
      char dirname[PATH_MAX];
      DIR *dir2;
      struct dirent *dent2;

      if (dent->d_name[0] == '.')
        continue;

      //snprintf(dirname, sizeof(dirname), "%s/%s/devices", base, dent->d_name);
      snprintf(dirname, sizeof(dirname), "%s/%s%s", base, dent->d_name, DEVICES_BASE);

      /* look for devices */
      dir2 = opendir(dirname);
      if (dir2 != NULL) {
        for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
          char dirname2[PATH_MAX];

          if (dent2->d_name[0] == '.')
            continue;

          snprintf(dirname2, sizeof(dirname2), "%s/%s", dirname, dent2->d_name);
          device_list_insert_op(dirname2, op);
        }
        closedir(dir2);
      }
    }
    closedir(dir);
  }
}

static void scan_block(int op)
{
  DIR *dir;
  struct dirent *dent;

  dir = opendir(BLOCK_BASE);
  if (dir != NULL) {
    for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
      char dirname[PATH_MAX];
      DIR *dir2;
      struct dirent *dent2;

      if (dent->d_name[0] == '.')
        continue;

      snprintf(dirname, sizeof(dirname), "%s/%s", BLOCK_BASE, dent->d_name);
      if (device_list_insert_op(dirname, op) != 0)
        continue;

      /* look for partitions */
      dir2 = opendir(dirname);
      if (dir2 != NULL) {
        for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
          char dirname2[PATH_MAX];

          if (dent2->d_name[0] == '.')
            continue;

          if (!strcmp(dent2->d_name,"device"))
            continue;

          snprintf(dirname2, sizeof(dirname2), "%s/%s", dirname, dent2->d_name);
          device_list_insert_op(dirname2, op);
        }
        closedir(dir2);
      }
    }
    closedir(dir);
  }
}

static void scan_class(int op)
{
  DIR *dir;
  struct dirent *dent;

  dir = opendir(CLASS_BASE);
  if (dir != NULL) {
    for (dent = readdir(dir); dent != NULL; dent = readdir(dir)) {
      char dirname[PATH_MAX];
      DIR *dir2;
      struct dirent *dent2;

      if (dent->d_name[0] == '.')
        continue;

      snprintf(dirname, sizeof(dirname), "%s/%s", CLASS_BASE, dent->d_name);
      dir2 = opendir(dirname);
      if (dir2 != NULL) {
        for (dent2 = readdir(dir2); dent2 != NULL; dent2 = readdir(dir2)) {
          char dirname2[PATH_MAX];

          if (dent2->d_name[0] == '.')
            continue;

          if (!strcmp(dent2->d_name, "device"))
            continue;

          snprintf(dirname2, sizeof(dirname2), "%s/%s", dirname, dent2->d_name);
          device_list_insert_op(dirname2, op);
        }
        closedir(dir2);
      }
    }
    closedir(dir);
  }
}

static int udev_trigger_pop(int op)
{
  /* if we have /sys/subsystem, forget all the old stuff */
  scan_subsystem("bus", op);
  scan_class(op);

  /* scan "block" if it isn't a "class" */
  if (access("/sys/class/block", F_OK) != 0) {
    scan_block(op);
  }
  return (EXIT_SUCCESS);
}

int utils_udev_trigger(void)
{
  return udev_trigger_pop(OP_TRIGGER);
}

int utils_udev_pop_nodes(void)
{
  return udev_trigger_pop(OP_POP_NODE);
}

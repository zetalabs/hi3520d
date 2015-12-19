/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox Configuration Daemon
 * Module Name  : utils_handle_kernel_module.c
 *
 * Description  : ezcfg install/remove kernel modules function
 *
 * Copyright (C) 2008-2013 by ezbox-project
 *
 * History      Rev       Description
 * 2011-07-28   0.1       Write it from scratch
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

typedef struct mod_dep_s {
  char *name;
  char *list;
} mod_dep_t;

/* directly depends list */
static mod_dep_t mod_depends[] = {
  { "aes_i586", "aes_generic" },
  { "ahci", "libahci" },
  { "arc4", "crypto_algapi" },
  { "ath5k", "ath,cfg80211,compat,mac80211" },
  { "authenc", "aead,crypto_algapi,crypto_blkcipher,crypto_hash" },
  { "cfg80211", "compat" },
  { "crc32c", "crypto_hash" },
  { "cryptomgr", "aead,crypto_algapi" },
  { "crypto_blkcipher", "crypto_algapi" },
  { "crypto_hash", "crypto_algapi" },
  { "crypto_null", "crypto_algapi,crypto_blkcipher,crypto_hash" },
  { "des_generic", "crypto_algapi" },
  { "ehci-hcd", "usbcore" },
  { "fat", "nls_cp437,nls_iso8859-1" },
  { "fcrypt", "crypto_algapi" },
  { "ohci-hcd", "nls_base,usbcore" },
  { "pcmcia", "pcmcia_core" },
  { "mac80211", "cfg80211,compat" },
  { "md4", "crypto_hash" },
  { "md5", "crypto_hash" },
  { "lib80211_crypt_ccmp", "lib80211" },
  { "lib80211_crypt_tkip", "lib80211" },
  { "lib80211_crypt_wep", "lib80211" },
  { "michael_mic", "crypto_hash" },
  { "nls_cp437", "nls_base" },
  { "nls_iso8859-1", "nls_base" },
  { "rsrc_nonstatic", "pcmcia_core" },
  { "rt2x00lib", "cfg80211,compat,compat_firmware_class,mac80211" },
  { "rt2x00usb", "mac80211,rt2x00lib,usbcore" },
  { "rt73usb", "crc-itu-t,rt2x00lib,rt2x00usb" },
  { "uhci-hcd", "usbcore" },
  { "usbcore", "usb-common,nls_base" },
  { "usbhid", "usbcore" },
  { "usb-storage", "usbcore" },
  { "vfat", "nls_base,nls_cp437,nls_iso8859-1,fat" },
  { "yenta_socket", "pcmcia_core,rsrc_nonstatic" },
};

int utils_install_kernel_module(char *name, char *args)
{
  mod_dep_t *mdp;
  FILE *file = NULL;
  char buf[128];
  char *p, *q, *l;
  char *kver;
  size_t i;
  int ret = EXIT_FAILURE;

  if (name == NULL) {
    return (EXIT_FAILURE);
  }

  /* check if it's been installed */
  file = fopen("/proc/modules", "r");
  if (file != NULL) {
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      p = strchr(buf, ' ');
      if (p != NULL) {
	*p = '\0';
      }
      if (strcmp(name, buf) == 0) {
	/* already installed the module */
	fclose(file);
	return (EXIT_SUCCESS);
      }
    }
    fclose(file);
  }

  /* first check if we should insmod related kernel modules */
  for (i = 0; i < ARRAY_SIZE(mod_depends); i++) {
    mdp = &mod_depends[i];
    if (strcmp(mdp->name, name) == 0) {
      l = strdup(mdp->list);
      if (l != NULL) {
	p = l;
	while(p != NULL) {
	  q = strchr(p, ',');
	  if (q != NULL)
	    *q = '\0';

	  /* check it recursively */
	  utils_install_kernel_module(p, NULL);

	  if (q != NULL)
	    p = q+1;
	  else
	    p = NULL;
	}
	free(l);
      }
    }
  }

  kver = utils_get_kernel_version();
  if (kver == NULL) {
    return ret;
  }

  /* then insmod the kernel module directly */
  q = (args == NULL) ? "" : args;
  i = strlen(CMD_INSMOD);
  i += 14; /* strlen(" /lib/modules/") */
  i += (strlen(kver) + 1); /* %s/ */
  i += (strlen(name) + 3);
  i += (strlen(q) + 2); /* " %s", one more for '\0' */
  p = malloc(i);
  if (p != NULL) {
    snprintf(p, i, "%s /lib/modules/%s/%s.ko %s", CMD_INSMOD, kver, name, q);
    utils_system(p);
    free(p);
  }
  free(kver);
  ret = EXIT_SUCCESS;

  return ret;
}

int utils_remove_kernel_module(char *name)
{
  mod_dep_t *mdp;
  //char buf[128];
  char *p, *q, *l;
  size_t i;
  int ret = EXIT_FAILURE;

  if (name == NULL) {
    return ret;
  }

  /* first rmmod the kernel module directly */
  i = strlen(CMD_RMMOD) + strlen(name) + 2;
  p = malloc(i);
  if (p == NULL) {
    return ret;
  }
  snprintf(p, i, "%s %s", CMD_RMMOD, name);
  utils_system(p);
  free(p);

  /* then check if we can rmmod related kernel modules */
  for (i = 0; i < ARRAY_SIZE(mod_depends); i++) {
    mdp = &mod_depends[i];
    if (strcmp(mdp->name, name) == 0) {
      l = strdup(mdp->list);
      if (l != NULL) {
	p = l;
	while(p != NULL) {
	  q = strrchr(p, ',');
	  if (q != NULL) {
	    p = q+1;
	  }

	  utils_remove_kernel_module(p);

	  if (q != NULL) {
	    *q = '\0';
	    p = l;
	  }
	  else {
	    p = NULL;
	  }
	}
	free(l);
      }
    }
  }

  ret = EXIT_SUCCESS;

  return ret;
}

int utils_probe_kernel_module(char *name, char *args)
{
  FILE *file = NULL;
  char buf[128];
  char *p, *q;
  int ret = EXIT_FAILURE;

  if (name == NULL) {
    return (EXIT_FAILURE);
  }

  /* check if it's been installed */
  file = fopen("/proc/modules", "r");
  if (file != NULL) {
    while (utils_file_get_line(file,
			       buf, sizeof(buf), "#", LINE_TAIL_STRING) == true) {
      p = strchr(buf, ' ');
      if (p != NULL) {
	*p = '\0';
      }
      if (strcmp(name, buf) == 0) {
	/* already installed the module */
	fclose(file);
	return (EXIT_SUCCESS);
      }
    }
    fclose(file);
  }

  /* modprobe kernel modules */
  q = (args == NULL) ? "" : args;
  snprintf(buf, sizeof(buf), "%s %s %s", CMD_MODPROBE, name, q);
  utils_system(buf);
  ret = EXIT_SUCCESS;

  return ret;
}

int utils_clean_kernel_module(char *name)
{
  char buf[128];
  int ret = EXIT_FAILURE;

  if (name == NULL) {
    return ret;
  }

  /* first rmmod the kernel module directly */
  snprintf(buf, sizeof(buf), "%s -r %s", CMD_MODPROBE, name);
  utils_system(buf);
  ret = EXIT_SUCCESS;

  return ret;
}

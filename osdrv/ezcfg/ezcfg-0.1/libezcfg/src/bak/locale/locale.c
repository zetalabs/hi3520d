/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : locale.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-02   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdarg.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#if (HAVE_EZBOX_EZCFG_NLS == 1)
#include <locale.h>
#include <libintl.h>
#endif

#include "ezcfg.h"
#include "ezcfg-private.h"

/**
 * The locale naming convention is:
 * language[_territory][.codeset][@modifier]
 * example: en_US.UTF-8
 */

struct ezcfg_locale {
	struct ezcfg   *ezcfg;
	char            name[EZCFG_LOCALE_MAX];
	char            domain[EZCFG_PATH_MAX];
	char            dir[EZCFG_PATH_MAX];
	pthread_mutex_t mutex; /* Protects locale */
};

struct ezcfg_locale * ezcfg_locale_new(struct ezcfg *ezcfg)
{
  struct ezcfg_locale *locale;

  ASSERT(ezcfg != NULL);

  locale = (struct ezcfg_locale *)malloc(sizeof(struct ezcfg_locale));
  if (locale == NULL) {
    return NULL;
  }
  memset(locale, 0, sizeof(struct ezcfg_locale));
  if (pthread_mutex_init(&locale->mutex, NULL) != 0) {
    free(locale);
    return NULL;
  }
  locale->ezcfg = ezcfg;
  locale->name[0] = '\0';
  locale->domain[0] = '\0';
  locale->dir[0] = '\0';

  return locale;
}

int ezcfg_locale_delete(struct ezcfg_locale *locale)
{
  if (locale != NULL) {
    free(locale);
  }

  return EZCFG_RET_OK;
}

int ezcfg_locale_set_name(struct ezcfg_locale *locale, char *name)
{
  //struct ezcfg *ezcfg;
  int n;

  ASSERT(locale != NULL);
  ASSERT(name != NULL);

  //ezcfg = locale->ezcfg;

  n = snprintf(locale->name, sizeof(locale->name), "%s", name);
  if ((n >= 0) && (strcmp(locale->name, name) == 0)) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

int ezcfg_locale_set_domain(struct ezcfg_locale *locale, char *domain)
{
  //struct ezcfg *ezcfg;
  int n;

  ASSERT(locale != NULL);
  ASSERT(domain != NULL);

  //ezcfg = locale->ezcfg;

  n = snprintf(locale->domain, sizeof(locale->domain), "%s", domain);
  if ((n >= 0) && (strcmp(locale->domain, domain) == 0)) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

int ezcfg_locale_set_dir(struct ezcfg_locale *locale, char *dir)
{
  //struct ezcfg *ezcfg;
  int n;

  ASSERT(locale != NULL);
  ASSERT(dir != NULL);

  //ezcfg = locale->ezcfg;

  n = snprintf(locale->dir, sizeof(locale->dir), "%s", dir);
  if ((n >= 0) && (strcmp(locale->dir, dir) == 0)) {
    return EZCFG_RET_OK;
  }
  else {
    return EZCFG_RET_FAIL;
  }
}

char * ezcfg_locale_text(struct ezcfg_locale *locale, char * msgid)
{
#if (HAVE_EZBOX_EZCFG_NLS == 1)
  struct ezcfg *ezcfg;
#endif
  char * p;

  ASSERT(msgid != NULL);

  /* no locale, return msgid directly */
  if (locale == NULL) {
    return msgid;
  }

#if (HAVE_EZBOX_EZCFG_NLS == 1)
  ezcfg = locale->ezcfg;

  /* lock locale mutex */
  pthread_mutex_lock(&locale->mutex);

  p = setlocale(LC_ALL, locale->name);
  if (p == NULL) {
    info(ezcfg, "setlocale error.\n");
  }

  /* set directory containing message catalogs */
  p = bindtextdomain(locale->domain, locale->dir);
  if (p == NULL) {
    info(ezcfg, "bindtextdomain error.\n");
  }

  /* set domain for future gettext() calls */
  p = textdomain(locale->domain);
  if (p == NULL) {
    info(ezcfg, "textdomain error.\n");
  }

  p = gettext(msgid);

  /* unlock locale mutex */
  pthread_mutex_unlock(&locale->mutex);
#else
  p = msgid;
#endif

  return p;
}

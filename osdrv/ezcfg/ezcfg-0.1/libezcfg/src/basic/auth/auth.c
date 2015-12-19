/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/auth/auth.c
 *
 * Description  : implement authentications
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2011-04-08   0.1       Write it from scratch
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"

struct ezcfg_auth {
  struct ezcfg *ezcfg;

  char *type;	/* type */
  char *user;	/* user */
  char *realm;	/* realm */
  char *domain;	/* domain */
  char *secret;	/* secret */

  struct ezcfg_auth *next; /* link pointer */
};

/*
 * Private functions
 */
static bool auth_http_basic_is_valid(struct ezcfg_auth *auth)
{
  if ((auth->user == NULL) ||
      (auth->realm == NULL) ||
      (auth->domain == NULL) ||
      (auth->secret == NULL)) {
    return false;
  }
  return true;
}

static bool auth_http_digest_is_valid(struct ezcfg_auth *auth)
{
  if ((auth->user == NULL) ||
      (auth->realm == NULL) ||
      (auth->domain == NULL) ||
      (auth->secret == NULL)) {
    return false;
  }
  return true;
}

static bool auth_is_same(struct ezcfg_auth *a1, struct ezcfg_auth *a2)
{
  if ((strcmp(a1->type, a2->type) == 0) &&
      (strcmp(a1->user, a2->user) == 0) &&
      (strcmp(a1->realm, a2->realm) == 0) &&
      (strcmp(a1->domain, a2->domain) == 0) &&
      (strcmp(a1->secret, a2->secret) == 0)) {
    return true;
  }
  return false;
}

/*
 * Public functions
 */

int ezcfg_auth_delete(struct ezcfg_auth *auth)
{
  struct ezcfg *ezcfg;

  ASSERT(auth != NULL);

  ezcfg = auth->ezcfg;

  if (auth->type != NULL) {
    free(auth->type);
  }

  if (auth->user != NULL) {
    free(auth->user);
  }

  if (auth->realm != NULL) {
    free(auth->realm);
  }

  if (auth->domain != NULL) {
    free(auth->domain);
  }

  if (auth->secret != NULL) {
    free(auth->secret);
  }

  free(auth);
  /* decrease ezcfg library context reference */
  if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_dec_ref() failed\n");
  }
  return EZCFG_RET_OK;
}

struct ezcfg_auth *ezcfg_auth_new(struct ezcfg *ezcfg)
{
  struct ezcfg_auth *auth;

  ASSERT(ezcfg != NULL);

  /* increase ezcfg library context reference */
  if (ezcfg_inc_ref(ezcfg) != EZCFG_RET_OK) {
    EZDBG("ezcfg_inc_ref() failed\n");
    return NULL;
  }

  auth = (struct ezcfg_auth *)calloc(1, sizeof(struct ezcfg_auth));
  if (auth == NULL) {
    err(ezcfg, "can not calloc auth\n");
    /* decrease ezcfg library context reference */
    if (ezcfg_dec_ref(ezcfg) != EZCFG_RET_OK) {
      EZDBG("ezcfg_dec_ref() failed\n");
    }
    return NULL;
  }

  auth->ezcfg = ezcfg;

  return auth;
}

bool ezcfg_auth_set_type(struct ezcfg_auth *auth, char *type)
{
  //struct ezcfg *ezcfg;
  char *p;

  ASSERT(auth != NULL);
  ASSERT(type != NULL);

  //ezcfg = auth->ezcfg;

  p = strdup(type);
  if (p == NULL) {
    return false;
  }
  if (auth->type != NULL) {
    free(auth->type);
  }
  auth->type = p;
  return true;
}

bool ezcfg_auth_set_user(struct ezcfg_auth *auth, char *user)
{
  //struct ezcfg *ezcfg;
  char *p;

  ASSERT(auth != NULL);
  ASSERT(user != NULL);

  //ezcfg = auth->ezcfg;

  p = strdup(user);
  if (p == NULL) {
    return false;
  }
  if (auth->user != NULL) {
    free(auth->user);
  }
  auth->user = p;
  return true;
}

bool ezcfg_auth_set_realm(struct ezcfg_auth *auth, char *realm)
{
  //struct ezcfg *ezcfg;
  char *p;

  ASSERT(auth != NULL);
  ASSERT(realm != NULL);

  //ezcfg = auth->ezcfg;

  p = strdup(realm);
  if (p == NULL) {
    return false;
  }
  if (auth->realm != NULL) {
    free(auth->realm);
  }
  auth->realm = p;
  return true;
}

bool ezcfg_auth_set_domain(struct ezcfg_auth *auth, char *domain)
{
  //struct ezcfg *ezcfg;
  char *p;

  ASSERT(auth != NULL);
  ASSERT(domain != NULL);

  //ezcfg = auth->ezcfg;

  p = strdup(domain);
  if (p == NULL) {
    return false;
  }
  if (auth->domain != NULL) {
    free(auth->domain);
  }
  auth->domain = p;
  return true;
}

bool ezcfg_auth_set_secret(struct ezcfg_auth *auth, char *secret)
{
  //struct ezcfg *ezcfg;
  char *p;

  ASSERT(auth != NULL);
  ASSERT(secret != NULL);

  //ezcfg = auth->ezcfg;

  p = strdup(secret);
  if (p == NULL) {
    return false;
  }
  if (auth->secret != NULL) {
    free(auth->secret);
  }
  auth->secret = p;
  return true;
}

bool ezcfg_auth_is_valid(struct ezcfg_auth *auth)
{
  //struct ezcfg *ezcfg;

  ASSERT(auth != NULL);

  //ezcfg = auth->ezcfg;

  if (auth->type == NULL) {
    return false;
  }

  if (strcmp(auth->type, EZCFG_AUTH_TYPE_HTTP_BASIC_STRING) == 0) {
    return auth_http_basic_is_valid(auth);
  }
  else if (strcmp(auth->type, EZCFG_AUTH_TYPE_HTTP_DIGEST_STRING) == 0) {
    return auth_http_digest_is_valid(auth);
  }
  else {
    return false;
  }
}

bool ezcfg_auth_list_in(struct ezcfg_auth **list, struct ezcfg_auth *auth)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_auth *ap;

  ASSERT(list != NULL);
  ASSERT(auth != NULL);

  //ezcfg = auth->ezcfg;

  ap = *list;
  while (ap != NULL) {
    if (auth_is_same(ap, auth) == true) {
      return true;
    }
    ap = ap->next;
  }
  return false;
}

bool ezcfg_auth_list_insert(struct ezcfg_auth **list, struct ezcfg_auth *auth)
{
  ASSERT(list != NULL);
  ASSERT(auth != NULL);

  auth->next = *list;
  *list = auth;
  return true;
}

void ezcfg_auth_list_delete(struct ezcfg_auth **list)
{
  struct ezcfg_auth *ap;

  ASSERT(list != NULL);

  ap = *list;
  while (ap != NULL) {
    *list = ap->next;
    ezcfg_auth_delete(ap);
    ap = *list;
  }
}

bool ezcfg_auth_check_authorized(struct ezcfg_auth **list, struct ezcfg_auth *auth)
{
  ASSERT(list != NULL);
  ASSERT(auth != NULL);

  if (auth->type == NULL) {
    /* unknown authentication type */
    return false;
  }
  return ezcfg_auth_list_in(list, auth);
}

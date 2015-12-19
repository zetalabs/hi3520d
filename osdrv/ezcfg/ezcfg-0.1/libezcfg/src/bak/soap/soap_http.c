/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : soap/soap_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-09-07   0.1       Write it from scratch
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
#include "ezcfg-soap_http.h"

#if 0
#define DBG(format, args...) do {			\
    char path[256];					\
    FILE *dbg_fp;					\
    snprintf(path, 256, "/tmp/%d-debug.txt", getpid()); \
    dbg_fp = fopen(path, "a");				\
    if (dbg_fp) {					\
      fprintf(dbg_fp, "tid=[%d] ", (int)gettid());	\
      fprintf(dbg_fp, format, ## args);			\
      fclose(dbg_fp);					\
    }							\
  } while(0)
#else
#define DBG(format, args...)
#endif


/* for SOAP/HTTP binding request methods */
static const char *soap_http_method_strings[] = {
  /* bad method string */
  NULL ,
  /* SOAP/HTTP binding used methods */
  EZCFG_SOAP_HTTP_METHOD_GET ,
  EZCFG_SOAP_HTTP_METHOD_POST ,
};

/* for SOAP/HTTP binding known header */
static const char *soap_http_header_strings[] = {
  /* bad header string */
  NULL ,
  /* SOAP/HTTP binding known headers */
  EZCFG_SOAP_HTTP_HEADER_HOST ,
  EZCFG_SOAP_HTTP_HEADER_CONTENT_TYPE ,
  EZCFG_SOAP_HTTP_HEADER_CONTENT_LENGTH ,
};

/**
 * Public functions
 **/
void ezcfg_soap_http_delete(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);

  //ezcfg = sh->ezcfg;

  if (sh->soap != NULL)
    ezcfg_soap_delete(sh->soap);

  if (sh->http != NULL)
    ezcfg_http_delete(sh->http);

  free(sh);
}

/**
 * ezcfg_soap_http_new:
 * Create ezcfg SOAP/HTTP info builder data structure
 * Returns: a new ezcfg soap http binding info builder data structure
 **/
struct ezcfg_soap_http *ezcfg_soap_http_new(struct ezcfg *ezcfg)
{
  struct ezcfg_soap_http *sh;

  ASSERT(ezcfg != NULL);

  /* initialize soap http binding info builder data structure */
  sh = calloc(1, sizeof(struct ezcfg_soap_http));
  if (sh == NULL) {
    err(ezcfg, "initialize soap http binding builder error.\n");
    return NULL;
  }

  memset(sh, 0, sizeof(struct ezcfg_soap_http));

  sh->soap = ezcfg_soap_new(ezcfg);
  if (sh->soap == NULL) {
    ezcfg_soap_http_delete(sh);
    return NULL;
  }

  sh->http = ezcfg_http_new(ezcfg);
  if (sh->http == NULL) {
    ezcfg_soap_http_delete(sh);
    return NULL;
  }

  sh->ezcfg = ezcfg;
  ezcfg_http_set_method_strings(sh->http, soap_http_method_strings, ARRAY_SIZE(soap_http_method_strings) - 1);
  ezcfg_http_set_known_header_strings(sh->http, soap_http_header_strings, ARRAY_SIZE(soap_http_header_strings) - 1);

  return sh;
}

unsigned short ezcfg_soap_http_get_soap_version_major(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_soap_get_version_major(sh->soap);
}

unsigned short ezcfg_soap_http_get_soap_version_minor(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_soap_get_version_minor(sh->soap);
}

bool ezcfg_soap_http_set_soap_version_major(struct ezcfg_soap_http *sh, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_soap_set_version_major(sh->soap, major);
}

bool ezcfg_soap_http_set_soap_version_minor(struct ezcfg_soap_http *sh, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_soap_set_version_minor(sh->soap, minor);
}

unsigned short ezcfg_soap_http_get_http_version_major(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_http_get_version_major(sh->http);
}

unsigned short ezcfg_soap_http_get_http_version_minor(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_http_get_version_minor(sh->http);
}

bool ezcfg_soap_http_set_http_version_major(struct ezcfg_soap_http *sh, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_http_set_version_major(sh->http, major);
}

bool ezcfg_soap_http_set_http_version_minor(struct ezcfg_soap_http *sh, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_http_set_version_minor(sh->http, minor);
}

struct ezcfg_soap *ezcfg_soap_http_get_soap(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  return sh->soap;
}

struct ezcfg_http *ezcfg_soap_http_get_http(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return sh->http;
}

char *ezcfg_soap_http_get_http_header_value(struct ezcfg_soap_http *sh, char *name)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;

  return ezcfg_http_get_header_value(sh->http, name);
}

void ezcfg_soap_http_reset_attributes(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;

  ezcfg_http_reset_attributes(sh->http);
  ezcfg_soap_reset_attributes(sh->soap);
}

void ezcfg_soap_http_dump(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;

  ASSERT(sh != NULL);

  //ezcfg = sh->ezcfg;
}

bool ezcfg_soap_http_parse_header(struct ezcfg_soap_http *sh, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;
  http = sh->http;

  if (ezcfg_http_parse_header(http, buf, len) == false) {
    return false;
  }

  return true;
}

bool ezcfg_soap_http_parse_message_body(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_soap *soap;
  char *msg_body;
  int msg_body_len;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);

  //ezcfg = sh->ezcfg;
  http = sh->http;
  soap = sh->soap;

  msg_body = ezcfg_http_get_message_body(http);
  msg_body_len = ezcfg_http_get_message_body_len(http);

  if (msg_body != NULL && msg_body_len > 0) {
    if (ezcfg_soap_parse(soap, msg_body, msg_body_len) == false) {
      return false;
    }
  }

  return true;
}

char *ezcfg_soap_http_set_message_body(struct ezcfg_soap_http *sh, const char *body, const int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;
  http = sh->http;

  return ezcfg_http_set_message_body(http, body, len);
}

int ezcfg_soap_http_get_message_length(struct ezcfg_soap_http *sh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  int n, count;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);

  //ezcfg = sh->ezcfg;
  http = sh->http;

  count = 0;
  n = ezcfg_http_get_start_line_length(http);
  if (n < 0) {
    return -1;
  }
  count += n;

  n = ezcfg_http_get_headers_length(http);
  if (n < 0) {
    return -1;
  }
  count += n;

  n = ezcfg_http_get_crlf_length(http);
  if (n < 0) {
    return -1;
  }
  count += n;

  n = ezcfg_http_get_message_body_len(http);
  if (n < 0) {
    return -1;
  }
  count += n;
  return count;
}

int ezcfg_soap_http_write_message(struct ezcfg_soap_http *sh, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  //struct ezcfg_soap *soap;

  char *p;
  int n;

  ASSERT(sh != NULL);
  ASSERT(sh->http != NULL);
  ASSERT(sh->soap != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = sh->ezcfg;
  http = sh->http;
  //soap = sh->soap;

  p = buf;
  n = ezcfg_http_write_start_line(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_start_line\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_headers(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_headers\n");
    return n;
  }
  p += n;
  len -= n;

  n = ezcfg_http_write_crlf(http, p, len);
  if (n < 0) {
    err(ezcfg, "ezcfg_http_write_crlf\n");
    return n;
  }
  p += n;
  len -= n;

  if (ezcfg_http_get_message_body(http) != NULL) {
    n = ezcfg_http_write_message_body(http, p, len);
    if (n < 0) {
      err(ezcfg, "ezcfg_http_write_message_body\n");
      return n;
    }
    p += n;
  }

  return (p-buf);
}

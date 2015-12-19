/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : json/json_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2014-03-14   0.1       Write it from scratch
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
#include "ezcfg-json_http.h"

/* for SOAP/HTTP binding request methods */
static const char *json_http_method_strings[] = {
  /* bad method string */
  NULL ,
  /* JSON over HTTP used methods */
  EZCFG_HTTP_METHOD_POST ,
};

/* for JSON over HTTP binding known header */
static const char *json_http_header_strings[] = {
  /* bad header string */
  NULL ,
  /* JSON over HTTP binding known headers */
  EZCFG_HTTP_HEADER_HOST ,
  EZCFG_HTTP_HEADER_CONTENT_TYPE ,
  EZCFG_HTTP_HEADER_CONTENT_LENGTH ,
};

/**
 * Public functions
 **/
void ezcfg_json_http_delete(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);

  //ezcfg = sh->ezcfg;

  if (jh->json != NULL)
    ezcfg_json_delete(jh->json);

  if (jh->http != NULL)
    ezcfg_http_delete(jh->http);

  free(jh);
}

/**
 * ezcfg_json_http_new:
 * Create ezcfg JSON over HTTP info builder data structure
 * Returns: a new ezcfg json http binding info builder data structure
 **/
struct ezcfg_json_http *ezcfg_json_http_new(struct ezcfg *ezcfg)
{
  struct ezcfg_json_http *jh;

  ASSERT(ezcfg != NULL);

  /* initialize json http binding info builder data structure */
  jh = calloc(1, sizeof(struct ezcfg_json_http));
  if (jh == NULL) {
    err(ezcfg, "initialize json http binding builder error.\n");
    return NULL;
  }

  jh->json = ezcfg_json_new(ezcfg);
  if (jh->json == NULL) {
    ezcfg_json_http_delete(jh);
    return NULL;
  }

  jh->http = ezcfg_http_new(ezcfg);
  if (jh->http == NULL) {
    ezcfg_json_http_delete(jh);
    return NULL;
  }

  jh->ezcfg = ezcfg;
  ezcfg_http_set_method_strings(jh->http, json_http_method_strings, ARRAY_SIZE(json_http_method_strings) - 1);
  ezcfg_http_set_known_header_strings(jh->http, json_http_header_strings, ARRAY_SIZE(json_http_header_strings) - 1);

  return jh;
}

unsigned short ezcfg_json_http_get_http_version_major(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return ezcfg_http_get_version_major(jh->http);
}

unsigned short ezcfg_json_http_get_http_version_minor(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return ezcfg_http_get_version_minor(jh->http);
}

bool ezcfg_json_http_set_http_version_major(struct ezcfg_json_http *jh, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return ezcfg_http_set_version_major(jh->http, major);
}

bool ezcfg_json_http_set_http_version_minor(struct ezcfg_json_http *jh, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return ezcfg_http_set_version_minor(jh->http, minor);
}

struct ezcfg_json *ezcfg_json_http_get_json(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->json != NULL);

  //ezcfg = jh->ezcfg;

  return jh->json;
}

struct ezcfg_http *ezcfg_json_http_get_http(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return jh->http;
}

char *ezcfg_json_http_get_http_header_value(struct ezcfg_json_http *jh, char *name)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;

  return ezcfg_http_get_header_value(jh->http, name);
}

void ezcfg_json_http_reset_attributes(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);
  ASSERT(jh->json != NULL);

  //ezcfg = jh->ezcfg;

  ezcfg_http_reset_attributes(jh->http);
  ezcfg_json_reset(jh->json);
}

void ezcfg_json_http_dump(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;

  ASSERT(jh != NULL);

  //ezcfg = jh->ezcfg;
}

bool ezcfg_json_http_parse_header(struct ezcfg_json_http *jh, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;
  http = jh->http;

  if (ezcfg_http_parse_header(http, buf, len) == false) {
    return false;
  }

  return true;
}

bool ezcfg_json_http_parse_message_body(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_json *json;
  char *msg_body;
  int msg_body_len;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);
  ASSERT(jh->json != NULL);

  //ezcfg = jh->ezcfg;
  http = jh->http;
  json = jh->json;

  msg_body = ezcfg_http_get_message_body(http);
  msg_body_len = ezcfg_http_get_message_body_len(http);

  if (msg_body != NULL && msg_body_len > 0) {
    if (ezcfg_json_parse_text(json, msg_body, msg_body_len) == EZCFG_RET_FAIL) {
      return false;
    }
  }

  return true;
}

char *ezcfg_json_http_set_message_body(struct ezcfg_json_http *jh, const char *body, const int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;
  http = jh->http;

  return ezcfg_http_set_message_body(http, body, len);
}

int ezcfg_json_http_get_message_length(struct ezcfg_json_http *jh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  int n, count;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);

  //ezcfg = jh->ezcfg;
  http = jh->http;

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

int ezcfg_json_http_write_message(struct ezcfg_json_http *jh, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  //struct ezcfg_json *json;

  char *p;
  int n;

  ASSERT(jh != NULL);
  ASSERT(jh->http != NULL);
  ASSERT(jh->json != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = jh->ezcfg;
  http = jh->http;
  //json= jh->json;

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

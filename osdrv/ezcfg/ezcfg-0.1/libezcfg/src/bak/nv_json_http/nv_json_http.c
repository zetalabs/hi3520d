/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : nv_json_http/nv_json_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Write it from scratch
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
#include "ezcfg-nv_json_http.h"

/* for SOAP/HTTP binding request methods */
static const char *nv_json_http_method_strings[] = {
  /* bad method string */
  NULL ,
  /* JSON over HTTP used methods */
  EZCFG_HTTP_METHOD_POST ,
};

/* for JSON over HTTP binding known header */
static const char *nv_json_http_header_strings[] = {
  /* bad header string */
  NULL ,
  /* JSON over HTTP binding known headers */
  EZCFG_HTTP_HEADER_HOST ,
  EZCFG_HTTP_HEADER_CONTENT_TYPE ,
  EZCFG_HTTP_HEADER_CONTENT_LENGTH ,
};

/**
 * Private functions
 **/
/* it must be nvram has a state coherence with list */
static int build_nv_json_http_response(
  struct ezcfg_nvram *nvram,
  struct ezcfg_linked_list * list,
  struct ezcfg_http *http)
{
  int rc = -1;

  ASSERT(http != NULL);

  if (ezcfg_nvram_set_entries(nvram, list) == true) {
    /* FIXME: name point to http->request_uri !!!
     * never reset http before using name */
    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 200);
    ezcfg_http_set_state_response(http);

    /* set return value */
    rc = 0;
  }

  return rc;
}

/**
 * Public functions
 **/
void ezcfg_nv_json_http_delete(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);

  if (njh->json != NULL)
    ezcfg_json_delete(njh->json);

  if (njh->http != NULL)
    ezcfg_http_delete(njh->http);

  free(njh);
}

/**
 * ezcfg_nv_json_http_new:
 * Create ezcfg NVRAM represented by JSON over HTTP info builder data structure
 * Returns: a new ezcfg nvram json http binding info builder data structure
 **/
struct ezcfg_nv_json_http *ezcfg_nv_json_http_new(struct ezcfg *ezcfg)
{
  struct ezcfg_nv_json_http *njh;

  ASSERT(ezcfg != NULL);

  /* initialize nvram json http binding info builder data structure */
  njh = calloc(1, sizeof(struct ezcfg_nv_json_http));
  if (njh == NULL) {
    err(ezcfg, "initialize nvram json http binding builder error.\n");
    return NULL;
  }

  njh->json = ezcfg_json_new(ezcfg);
  if (njh->json == NULL) {
    ezcfg_nv_json_http_delete(njh);
    return NULL;
  }

  njh->http = ezcfg_http_new(ezcfg);
  if (njh->http == NULL) {
    ezcfg_nv_json_http_delete(njh);
    return NULL;
  }

  njh->ezcfg = ezcfg;
  ezcfg_http_set_method_strings(njh->http, nv_json_http_method_strings, ARRAY_SIZE(nv_json_http_method_strings) - 1);
  ezcfg_http_set_known_header_strings(njh->http, nv_json_http_header_strings, ARRAY_SIZE(nv_json_http_header_strings) - 1);

  return njh;
}

unsigned short ezcfg_nv_json_http_get_http_version_major(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return ezcfg_http_get_version_major(njh->http);
}

unsigned short ezcfg_nv_json_http_get_http_version_minor(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return ezcfg_http_get_version_minor(njh->http);
}

bool ezcfg_nv_json_http_set_http_version_major(struct ezcfg_nv_json_http *njh, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return ezcfg_http_set_version_major(njh->http, major);
}

bool ezcfg_nv_json_http_set_http_version_minor(struct ezcfg_nv_json_http *njh, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return ezcfg_http_set_version_minor(njh->http, minor);
}

struct ezcfg_json *ezcfg_nv_json_http_get_json(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->json != NULL);

  //ezcfg = njh->ezcfg;

  return njh->json;
}

struct ezcfg_http *ezcfg_nv_json_http_get_http(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return njh->http;
}

char *ezcfg_nv_json_http_get_http_header_value(struct ezcfg_nv_json_http *njh, char *name)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;

  return ezcfg_http_get_header_value(njh->http, name);
}

void ezcfg_nv_json_http_reset_attributes(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);
  ASSERT(njh->json != NULL);

  //ezcfg = njh->ezcfg;

  ezcfg_http_reset_attributes(njh->http);
  ezcfg_json_reset(njh->json);
}

void ezcfg_nv_json_http_dump(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;

  ASSERT(njh != NULL);

  //ezcfg = njh->ezcfg;
}

bool ezcfg_nv_json_http_parse_header(struct ezcfg_nv_json_http *njh, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;
  http = njh->http;

  if (ezcfg_http_parse_header(http, buf, len) == false) {
    return false;
  }

  return true;
}

bool ezcfg_nv_json_http_parse_message_body(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  struct ezcfg_json *json;
  char *msg_body;
  int msg_body_len;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);
  ASSERT(njh->json != NULL);

  //ezcfg = jh->ezcfg;
  http = njh->http;
  json = njh->json;

  msg_body = ezcfg_http_get_message_body(http);
  msg_body_len = ezcfg_http_get_message_body_len(http);

  if (msg_body != NULL && msg_body_len > 0) {
    if (ezcfg_json_parse_text(json, msg_body, msg_body_len) == EZCFG_RET_FAIL) {
      return false;
    }
  }

  return true;
}

char *ezcfg_nv_json_http_set_message_body(struct ezcfg_nv_json_http *njh, const char *body, const int len)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;
  http = njh->http;

  return ezcfg_http_set_message_body(http, body, len);
}

int ezcfg_nv_json_http_get_message_length(struct ezcfg_nv_json_http *njh)
{
  //struct ezcfg *ezcfg;
  struct ezcfg_http *http;

  int n, count;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);

  //ezcfg = njh->ezcfg;
  http = njh->http;

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

int ezcfg_nv_json_http_write_message(struct ezcfg_nv_json_http *njh, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct ezcfg_http *http;
  //struct ezcfg_json *json;

  char *p;
  int n;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);
  ASSERT(njh->json != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = njh->ezcfg;
  http = njh->http;
  //json= njh->json;

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

int ezcfg_nv_json_http_handle_request(struct ezcfg_nv_json_http *njh)
{
  struct ezcfg_http *http;
  struct ezcfg_json *json;
  struct ezcfg_nvram *nvram;
  struct ezcfg_linked_list *list;
  int ret = EZCFG_RET_FAIL;

  ASSERT(njh != NULL);
  ASSERT(njh->http != NULL);
  ASSERT(njh->json != NULL);
  ASSERT(njh->nvram != NULL);

  http = njh->http;
  json = njh->json;
  nvram = njh->nvram;

  /* json contains parsed text info */
  list = ezcfg_json_build_nvram_node_list(json);
  if (list != NULL) {
    ret = build_nv_json_http_response(nvram, list, http);
    ezcfg_linked_list_delete(list);
  }

  return ret;
}

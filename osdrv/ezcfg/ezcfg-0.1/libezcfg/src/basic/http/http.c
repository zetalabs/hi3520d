/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/http/http.c
 *
 * Description  : implement HTTP/1.1 protocol (RFC 2616)
 *                and it's extension (RFC 2774)
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-07-12   0.1       Write it from scratch
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
#include <pthread.h>

#include "ezcfg.h"
#include "ezcfg-private.h"
#include "ezcfg-http.h"

struct http_header {
  char *name; /* HTTP header name */
  char *value; /* HTTP header value */
  struct http_header *next;
  bool is_known_header; /* true if name in header_strings */
};

struct status_code_reason_phrase_map {
  unsigned short status_code;
  char *reason_phrase;
};

struct ezcfg_http {
  struct ezcfg *ezcfg;

  http_state_t state;

  unsigned char num_methods; /* Number of supported methods */
  const char **method_strings;
  unsigned char method_index; /* index of method_strings, should be > 0, 0 means error */

  char *request_uri; /* URL-decoded URI */

  unsigned short version_major;
  unsigned short version_minor;

  unsigned short num_status_codes; /* Number of supported status codes */
  const struct status_code_reason_phrase_map *status_code_maps;
  unsigned short status_code_index; /* index of status_code_reason_phrase_map, should be > 0 */

  unsigned char num_known_headers; /* Number of supported headers */
  const char **known_header_strings;

  struct http_header *header_head; /* HTTP header list head */
  struct http_header *header_tail; /* HTTP header list tail */

  int message_body_len; /* HTTP message body length */
  char *message_body; /* HTTP message bosy buffer */

  bool is_ssl; /* true if SSL-ed, false if not */
  bool is_extension; /* true if HTTP extension framework */
};

static const char *default_method_strings[] = {
  /* bad method string */
  NULL ,
  /* HTTP/1.1 (RFC2616) defined methods */
  EZCFG_HTTP_METHOD_OPTIONS ,
  EZCFG_HTTP_METHOD_GET ,
  EZCFG_HTTP_METHOD_HEAD ,
  EZCFG_HTTP_METHOD_POST ,
  EZCFG_HTTP_METHOD_PUT ,
  EZCFG_HTTP_METHOD_DELETE ,
  EZCFG_HTTP_METHOD_TRACE ,
  EZCFG_HTTP_METHOD_CONNECT ,
  /* HTTP/1.1 extension (RFC2774) defined methods */
  EZCFG_HTTP_METHOD_OPTIONS_EXT ,
  EZCFG_HTTP_METHOD_GET_EXT ,
  EZCFG_HTTP_METHOD_HEAD_EXT ,
  EZCFG_HTTP_METHOD_POST_EXT ,
  EZCFG_HTTP_METHOD_PUT_EXT ,
  EZCFG_HTTP_METHOD_DELETE_EXT ,
  EZCFG_HTTP_METHOD_TRACE_EXT ,
  EZCFG_HTTP_METHOD_CONNECT_EXT ,
};

static const char *default_header_strings[] = {
  /* bad header string */
  NULL ,
  /* HTTP/1.1 (RFC2616) General headers */
  EZCFG_HTTP_HEADER_CACHE_CONTROL ,
  EZCFG_HTTP_HEADER_CONNECTION ,
  EZCFG_HTTP_HEADER_DATE ,
  EZCFG_HTTP_HEADER_PRAGMA ,
  EZCFG_HTTP_HEADER_TRAILER ,
  EZCFG_HTTP_HEADER_TRANSFER_ENCODING ,
  EZCFG_HTTP_HEADER_UPGRADE ,
  EZCFG_HTTP_HEADER_VIA ,
  EZCFG_HTTP_HEADER_WARNING ,
  /* HTTP/1.1 (RFC2616) request headers */
  EZCFG_HTTP_HEADER_ACCEPT ,
  EZCFG_HTTP_HEADER_ACCEPT_CHARSET ,
  EZCFG_HTTP_HEADER_ACCEPT_ENCODING ,
  EZCFG_HTTP_HEADER_ACCEPT_LANGUAGE ,
  EZCFG_HTTP_HEADER_AUTHORIZATION ,
  EZCFG_HTTP_HEADER_EXPECT ,
  EZCFG_HTTP_HEADER_FROM ,
  EZCFG_HTTP_HEADER_HOST ,
  EZCFG_HTTP_HEADER_IF_MATCH ,
  EZCFG_HTTP_HEADER_IF_MODIFIED_SINCE ,
  EZCFG_HTTP_HEADER_IF_NONE_MATCH ,
  EZCFG_HTTP_HEADER_IF_RANGE ,
  EZCFG_HTTP_HEADER_IF_UNMODIFIED_SINCE ,
  EZCFG_HTTP_HEADER_MAX_FORWARDS ,
  EZCFG_HTTP_HEADER_PROXY_AUTHORIZATION ,
  EZCFG_HTTP_HEADER_RANGE ,
  EZCFG_HTTP_HEADER_REFERER ,
  EZCFG_HTTP_HEADER_TE ,
  EZCFG_HTTP_HEADER_USER_AGENT ,
  /* HTTP/1.1 (RFC2616) response headers */
  EZCFG_HTTP_HEADER_ACCEPT_RANGES ,
  EZCFG_HTTP_HEADER_AGE ,
  EZCFG_HTTP_HEADER_ETAG ,
  EZCFG_HTTP_HEADER_LOCATION ,
  EZCFG_HTTP_HEADER_PROXY_AUTHENTICATE ,
  EZCFG_HTTP_HEADER_RETRY_AFTER ,
  EZCFG_HTTP_HEADER_SERVER ,
  EZCFG_HTTP_HEADER_VARY ,
  EZCFG_HTTP_HEADER_WWW_AUTHENTICATE ,
  /* HTTP/1.1 (RFC2616) entity headers */
  EZCFG_HTTP_HEADER_ALLOW ,
  EZCFG_HTTP_HEADER_CONTENT_ENCODING ,
  EZCFG_HTTP_HEADER_CONTENT_LANGUAGE ,
  EZCFG_HTTP_HEADER_CONTENT_LENGTH ,
  EZCFG_HTTP_HEADER_CONTENT_LOCATION ,
  EZCFG_HTTP_HEADER_CONTENT_MD5 ,
  EZCFG_HTTP_HEADER_CONTENT_RANGE ,
  EZCFG_HTTP_HEADER_CONTENT_TYPE ,
  EZCFG_HTTP_HEADER_EXPIRES ,
  EZCFG_HTTP_HEADER_LAST_MODIFIED ,
  /* HTTP/1.1 extension (RFC2774) headers */
  EZCFG_HTTP_HEADER_MAN ,
  EZCFG_HTTP_HEADER_OPT ,
  EZCFG_HTTP_HEADER_C_MAN ,
  EZCFG_HTTP_HEADER_C_OPT ,
  /* HTTP/1.1 extension (RFC2774) responese headers */
  EZCFG_HTTP_HEADER_EXT ,
  EZCFG_HTTP_HEADER_C_EXT ,
};

static const struct status_code_reason_phrase_map default_status_code_maps[] = {
  /* bad method string */
  { 0 , NULL } ,
  { 100 , EZCFG_HTTP_REASON_PHRASE_100 } ,
  { 101 , EZCFG_HTTP_REASON_PHRASE_101 } ,
  { 102 , EZCFG_HTTP_REASON_PHRASE_102 } ,
  { 200 , EZCFG_HTTP_REASON_PHRASE_200 } ,
  { 201 , EZCFG_HTTP_REASON_PHRASE_201 } ,
  { 202 , EZCFG_HTTP_REASON_PHRASE_202 } ,
  { 203 , EZCFG_HTTP_REASON_PHRASE_203 } ,
  { 204 , EZCFG_HTTP_REASON_PHRASE_204 } ,
  { 205 , EZCFG_HTTP_REASON_PHRASE_205 } ,
  { 206 , EZCFG_HTTP_REASON_PHRASE_206 } ,
  { 207 , EZCFG_HTTP_REASON_PHRASE_207 } ,
  { 300 , EZCFG_HTTP_REASON_PHRASE_300 } ,
  { 301 , EZCFG_HTTP_REASON_PHRASE_301 } ,
  { 302 , EZCFG_HTTP_REASON_PHRASE_302 } ,
  { 303 , EZCFG_HTTP_REASON_PHRASE_303 } ,
  { 304 , EZCFG_HTTP_REASON_PHRASE_304 } ,
  { 305 , EZCFG_HTTP_REASON_PHRASE_305 } ,
  { 306 , EZCFG_HTTP_REASON_PHRASE_306 } ,
  { 307 , EZCFG_HTTP_REASON_PHRASE_307 } ,
  { 400 , EZCFG_HTTP_REASON_PHRASE_400 } ,
  { 401 , EZCFG_HTTP_REASON_PHRASE_401 } ,
  { 402 , EZCFG_HTTP_REASON_PHRASE_402 } ,
  { 403 , EZCFG_HTTP_REASON_PHRASE_403 } ,
  { 404 , EZCFG_HTTP_REASON_PHRASE_404 } ,
  { 405 , EZCFG_HTTP_REASON_PHRASE_405 } ,
  { 406 , EZCFG_HTTP_REASON_PHRASE_406 } ,
  { 407 , EZCFG_HTTP_REASON_PHRASE_407 } ,
  { 408 , EZCFG_HTTP_REASON_PHRASE_408 } ,
  { 409 , EZCFG_HTTP_REASON_PHRASE_409 } ,
  { 410 , EZCFG_HTTP_REASON_PHRASE_410 } ,
  { 411 , EZCFG_HTTP_REASON_PHRASE_411 } ,
  { 412 , EZCFG_HTTP_REASON_PHRASE_412 } ,
  { 413 , EZCFG_HTTP_REASON_PHRASE_413 } ,
  { 414 , EZCFG_HTTP_REASON_PHRASE_414 } ,
  { 415 , EZCFG_HTTP_REASON_PHRASE_415 } ,
  { 416 , EZCFG_HTTP_REASON_PHRASE_416 } ,
  { 417 , EZCFG_HTTP_REASON_PHRASE_417 } ,
  { 422 , EZCFG_HTTP_REASON_PHRASE_422 } ,
  { 423 , EZCFG_HTTP_REASON_PHRASE_423 } ,
  { 424 , EZCFG_HTTP_REASON_PHRASE_424 } ,
  { 426 , EZCFG_HTTP_REASON_PHRASE_426 } ,
  { 500 , EZCFG_HTTP_REASON_PHRASE_500 } ,
  { 501 , EZCFG_HTTP_REASON_PHRASE_501 } ,
  { 502 , EZCFG_HTTP_REASON_PHRASE_502 } ,
  { 503 , EZCFG_HTTP_REASON_PHRASE_503 } ,
  { 504 , EZCFG_HTTP_REASON_PHRASE_504 } ,
  { 505 , EZCFG_HTTP_REASON_PHRASE_505 } ,
  { 507 , EZCFG_HTTP_REASON_PHRASE_507 } ,
};

/**
 * Private functions
 **/

static char *get_http_header_value(struct ezcfg_http *http, char *name)
{
  //struct ezcfg *ezcfg;
  struct http_header *h;

  ASSERT(http != NULL);
  ASSERT(name != NULL);

  //ezcfg = http->ezcfg;

  h = http->header_head;
  while(h != NULL) {
    if (strcasecmp(name, h->name) == 0)
      return h->value;
    h = h->next;
  }

  return NULL;
}

static void delete_http_header(struct http_header *h)
{
  if (h->is_known_header == false) {
    free(h->name);
  }
  if (h->value != NULL) {
    free(h->value);
  }
  free(h);
}

static void clear_http_headers(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;
  struct http_header *h;

  //ezcfg = http->ezcfg;
  while(http->header_head != NULL) {
    h = http->header_head;
    http->header_head = h->next;
    delete_http_header(h);
  }
  http->header_tail = NULL;
}

static char *find_known_header_name(struct ezcfg_http *http, char *name)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(http != NULL);
  ASSERT(name != NULL);

  //ezcfg = http->ezcfg;

  for (i = http->num_known_headers; i > 0; i--) {
    if (strcasecmp(http->known_header_strings[i], name) == 0) {
      return (char *)http->known_header_strings[i];
    }
  }
  return NULL;
}

/**
 * Parse HTTP headers from the given buffer, advance buffer to the point
 * where parsing stopped.
 **/
static bool parse_http_headers(struct ezcfg_http *http, char *buf)
{

  struct ezcfg *ezcfg;
  char *end, *name, *value;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);

  ezcfg = http->ezcfg;

  name = buf;
  /* header end with CRLF */
  end = strstr(name, EZCFG_HTTP_CRLF_STRING);
  if (end == NULL) {
    err(ezcfg, "HTTP header parse error.\n");
    return false;
  }
  *end = '\0';

  while (name[0] != '\0') {
    value = strchr(name, ':'); /* format is [name:value] */
    if (value == NULL) {
      err(ezcfg, "HTTP header parse error 2.\n");
      return false;
    }
    *value = '\0'; /* 0-terminated name string */
    /* find header value field */
    value++;
    value = ezcfg_util_skip_leading_charlist(value, " \t"); /* skip LWS */

    if (ezcfg_http_add_header(http, name, value) == false) {
      err(ezcfg, "HTTP add header error.\n");
      return false;
    }

    name = end+2; /* skip CRLF */
    end = strstr(name, EZCFG_HTTP_CRLF_STRING);
    if (end == NULL) {
      err(ezcfg, "HTTP header parse error 3.\n");
      return false;
    }
    *end = '\0';
  };

  return true;
}

static unsigned char find_method_index(struct ezcfg_http *http, const char *method)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(http != NULL);
  ASSERT(method != NULL);

  //ezcfg = http->ezcfg;

  for (i = http->num_methods; i > 0; i--) {
    if (strcmp(http->method_strings[i], method) == 0)
      return i;
  }
  return 0;
}

static unsigned short find_status_code_index(struct ezcfg_http *http, const unsigned short status_code)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(http != NULL);
  ASSERT(status_code > 0);

  //ezcfg = http->ezcfg;

  for (i = http->num_status_codes; i > 0; i--) {
    if (http->status_code_maps[i].status_code == status_code) {
      return i;
    }
  }
  return 0;
}

static bool parse_http_request(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;
  char *p;
  char *method, *uri, *version, *headers;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  method = buf;

  /* split HTTP Request-Line (RFC2616 section 5.1) */
  p = strstr(buf, EZCFG_HTTP_CRLF_STRING);
  if (p == NULL) {
    err(ezcfg, "no HTTP request line\n");
    return false;
  }

  headers = p+2; /* skip CRLF */
  *p = '\0';

  /* get request method string */
  p = strchr(method, ' ');
  if (p == NULL)
    return false;

  /* 0-terminated method string */
  *p = '\0';
  http->method_index = find_method_index(http, method);
  if (http->method_index == 0) {
    *p = ' ';
    return false;
  }

  /* restore the SP charactor */
  *p = ' ';

  /* get uri string */
  uri = p+1;
  if (uri[0] != '/' && uri[0] != '*')
    return false;

  p = strchr(uri, ' ');
  if (p == NULL)
    return false;

  /* 0-terminated request uri string */
  *p = '\0';
  http->request_uri = strdup(uri);
  if (http->request_uri == NULL)
    return false;

  /* restore the SP charactor */
  *p = ' ';

  /* get http version */
  version = p+1;
  if (strncmp(version, "HTTP/", 5) != 0)
    return false;

  p = version+5;
  if (sscanf(p, "%hd.%hd",
	     &http->version_major,
	     &http->version_minor) != 2)
    return false;

  /* parse http headers */
  return parse_http_headers(http, headers);
}

static bool parse_http_response(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;
  char *p;
  char *version, *reason_phrase, *headers;
  unsigned short status_code;
  int i;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  version = buf;

  /* split HTTP Status-Line (RFC2616 section 6.1) */
  p = strstr(buf, EZCFG_HTTP_CRLF_STRING);
  if (p == NULL) {
    err(ezcfg, "no HTTP status line\n");
    return false;
  }

  headers = p+2; /* skip CRLF */
  *p = '\0';

  /* get version string */
  p = strchr(version, ' ');
  if (p == NULL) {
    return false;
  }

  /* 0-terminated version string */
  *p = '\0';
  if (strncmp(version, "HTTP/", 5) != 0) {
    return false;
  }

  if (sscanf(version+5, "%hd.%hd",
	     &http->version_major,
	     &http->version_minor) != 2) {
    return false;
  }

  /* restore the SP charactor */
  *p = ' ';

  /* get status code string */
  p++;
  if (isdigit(p[0]) == 0 ||
      isdigit(p[1]) == 0 ||
      isdigit(p[2]) == 0 ||
      p[3] != ' ') {
    return false;
  }

  status_code = (p[0] - '0') * 100
    + (p[1] - '0') * 10
    + (p[2] - '0');

  /* skip status code string, and get reason phrase string */
  reason_phrase = p+4;

  i = find_status_code_index(http, status_code);
  if (i == 0) {
    return false;
  }

  if (strcasecmp(reason_phrase, http->status_code_maps[i].reason_phrase) != 0) {
    return false;
  }

  http->status_code_index = i;

  /* parse http headers */
  return parse_http_headers(http, headers);
}

/**
 * Public functions
 **/
int ezcfg_http_del(struct ezcfg_http *http)
{
  ASSERT(http != NULL);

  clear_http_headers(http);

  if (http->request_uri != NULL) {
    free(http->request_uri);
  }
  if (http->message_body != NULL) {
    free(http->message_body);
  }

  free(http);
  return EZCFG_RET_OK;
}

/**
 * ezcfg_http_new:
 *
 * Create ezcfg http info structure.
 *
 * Returns: a new ezcfg http info structure
 **/
struct ezcfg_http *ezcfg_http_new(struct ezcfg *ezcfg)
{
  struct ezcfg_http *http;

  ASSERT(ezcfg != NULL);

  /* initialize http info structure */
  http = calloc(1, sizeof(struct ezcfg_http));
  if (http == NULL) {
    return NULL;
  }

  memset(http, 0, sizeof(struct ezcfg_http));

  http->ezcfg = ezcfg;
  http->num_methods = ARRAY_SIZE(default_method_strings) - 1; /* first item is NULL */
  http->method_strings = default_method_strings;

  http->num_known_headers = ARRAY_SIZE(default_header_strings) - 1; /* first item is NULL */
  http->known_header_strings = default_header_strings;

  http->num_status_codes = ARRAY_SIZE(default_status_code_maps) - 1; /* first item is NULL */
  http->status_code_maps = default_status_code_maps;

  http->header_head = NULL;
  http->header_tail = NULL;

  return http;
}

int ezcfg_http_set_state_request(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  http->state = HTTP_REQUEST;
  return EZCFG_RET_OK;
}

int ezcfg_http_set_state_response(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  http->state = HTTP_RESPONSE;
  return EZCFG_RET_OK;
}

bool ezcfg_http_is_state_request(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  if (http->state == HTTP_REQUEST)
    return true;
  else
    return false;
}

bool ezcfg_http_is_state_response(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  if (http->state == HTTP_RESPONSE)
    return true;
  else
    return false;
}

int ezcfg_http_reset_attributes(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  clear_http_headers(http);

  if (http->request_uri != NULL) {
    free(http->request_uri);
    http->request_uri = NULL;
  }

  if (http->message_body != NULL) {
    free(http->message_body);
    http->message_body = NULL;
    http->message_body_len = 0;
  }
  return EZCFG_RET_OK;
}

bool ezcfg_http_parse_header(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  if (http->state == HTTP_REQUEST) {
    return parse_http_request(http, buf, len);
  }
  else if (http->state == HTTP_RESPONSE) {
    return parse_http_response(http, buf, len);
  }
  else {
    err(ezcfg, "Unknown HTTP state.\n");
    return false;
  }
}

struct ezcfg *ezcfg_http_get_ezcfg(struct ezcfg_http *http)
{
  ASSERT(http != NULL);

  return http->ezcfg;
}

unsigned short ezcfg_http_get_version_major(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->version_major;
}

unsigned short ezcfg_http_get_version_minor(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->version_minor;
}

bool ezcfg_http_set_version_major(struct ezcfg_http *http, unsigned short major)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  http->version_major = major;

  return true;
}

bool ezcfg_http_set_version_minor(struct ezcfg_http *http, unsigned short minor)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  http->version_minor = minor;

  return true;
}

char *ezcfg_http_get_header_value(struct ezcfg_http *http, char *name)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(name != NULL);

  //ezcfg = http->ezcfg;

  return get_http_header_value(http, name);
}

int ezcfg_http_dump(struct ezcfg_http *http)
{
  struct ezcfg *ezcfg;
  struct http_header *h;
  unsigned short status_code;
  char *reason_phrase;

  ASSERT(http != NULL);

  ezcfg = http->ezcfg;
  status_code = http->status_code_maps[http->status_code_index].status_code;
  reason_phrase = http->status_code_maps[http->status_code_index].reason_phrase;

  info(ezcfg, "request_method=[%s]\n", http->method_strings[http->method_index]);
  info(ezcfg, "request_uri=[%s]\n", http->request_uri);
  info(ezcfg, "http_version=[%d.%d]\n", http->version_major, http->version_minor);
  info(ezcfg, "message_body_len=[%d]\n", http->message_body_len);
  info(ezcfg, "message_body=[%s]\n", http->message_body);
  info(ezcfg, "status_code=[%d]\n", status_code);
  info(ezcfg, "reason_phrase=[%s]\n", reason_phrase);
  info(ezcfg, "is_ssl=[%d]\n", http->is_ssl);
  h = http->header_head;
  while (h != NULL) {
    info(ezcfg, "header(known=%d)=[%s:%s]\n", h->is_known_header, h->name, h->value);
    h = h->next;
  }
  return EZCFG_RET_OK;
}

bool ezcfg_http_set_method_strings(struct ezcfg_http *http, const char **method_strings, unsigned char num_methods)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(method_strings != NULL);

  //ezcfg = http->ezcfg;

  http->num_methods = num_methods;
  http->method_strings = method_strings;

  return true;
}

bool ezcfg_http_set_known_header_strings(struct ezcfg_http *http, const char **header_strings, unsigned char num_headers)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(header_strings != NULL);

  //ezcfg = http->ezcfg;

  http->num_known_headers = num_headers;
  http->known_header_strings = header_strings;

  return true;
}

bool ezcfg_http_set_status_code_maps(struct ezcfg_http *http, const void *maps, unsigned short num_status_codes)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(maps != NULL);

  //ezcfg = http->ezcfg;

  http->num_status_codes = num_status_codes;
  http->status_code_maps = maps;

  return true;
}

unsigned char ezcfg_http_set_request_method(struct ezcfg_http *http, const char *method)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(http != NULL);
  ASSERT(method != NULL);

  //ezcfg = http->ezcfg;

  for (i = http->num_methods; i > 0; i--) {
    if (strcmp(http->method_strings[i], method) == 0) {
      http->method_index = i;
      if (method[0] == 'M' && method[1] == '-') {
	http->is_extension = true;
      }
      return i;
    }
  }

  return 0;
}

int ezcfg_http_request_method_cmp(struct ezcfg_http *http, const char *method)
{
  return strcmp(http->method_strings[http->method_index], method);
}

unsigned short ezcfg_http_set_status_code(struct ezcfg_http *http, unsigned short status_code)
{
  //struct ezcfg *ezcfg;
  int i;

  ASSERT(http != NULL);
  ASSERT(status_code != 0);

  //ezcfg = http->ezcfg;

  for (i = http->num_status_codes; i > 0; i--) {
    if (http->status_code_maps[i].status_code == status_code) {
      http->status_code_index = i;
      return i;
    }
  }

  return 0;
}

char *ezcfg_http_get_request_uri(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->request_uri;
}

bool ezcfg_http_set_request_uri(struct ezcfg_http *http, const char *uri)
{
  //struct ezcfg *ezcfg;
  char *request_uri;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  request_uri = strdup(uri);
  if (request_uri == NULL) {
    return false;
  }

  if (http->request_uri != NULL) {
    free(http->request_uri);
  }

  http->request_uri = request_uri;

  return true;
}

bool ezcfg_http_get_is_ssl(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->is_ssl;
}

bool ezcfg_http_set_is_ssl(struct ezcfg_http *http, const bool is_ssl)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  http->is_ssl = is_ssl;

  return true;
}

/* \0-terminated the message body */
char *ezcfg_http_set_message_body(struct ezcfg_http *http, const char *body, const int len)
{
  //struct ezcfg *ezcfg;
  char *message_body;

  ASSERT(http != NULL);
  ASSERT(body != NULL);
  ASSERT(len > 0);

  //ezcfg = http->ezcfg;

  message_body = malloc(len+1);
  if (message_body == NULL) {
    return NULL;
  }

  if (len > 0) {
    memcpy(message_body, body, len);
  }
  message_body[len] = '\0';

  if (http->message_body != NULL) {
    free(http->message_body);
  }

  http->message_body = message_body;
  http->message_body_len = len;

  return message_body;
}

int ezcfg_http_get_message_body_len(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->message_body_len;
}

char *ezcfg_http_get_message_body(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return http->message_body;
}

int ezcfg_http_get_request_line_length(struct ezcfg_http *http)
{
  struct ezcfg *ezcfg;
  int n, count;

  ASSERT(http != NULL);

  ezcfg = http->ezcfg;

  if (http->method_index == 0) {
    err(ezcfg, "unknown http method\n");
    return -1;
  }

  count = 0;
  n = strlen(http->method_strings[http->method_index]);
  count += n;
  count ++; /* SP */
  n = strlen(http->request_uri);
  count += n;
  count ++; /* SP */
  count += 5; /* "HTTP/" */
  n = http->version_major;
  do {
    count++;
    n /= 10;
  } while (n > 0);
  count++; /* "." */
  n = http->version_minor;
  do {
    count++;
    n /= 10;
  } while (n > 0);
  n = strlen(EZCFG_HTTP_CRLF_STRING);
  count += n;

  return count;
}

int ezcfg_http_write_request_line(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;
  int n;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  if (http->method_index == 0) {
    err(ezcfg, "unknown http method\n");
    return -1;
  }

  n = snprintf(buf, len, "%s %s HTTP/%d.%d%s",
	       http->method_strings[http->method_index],
	       http->request_uri,
	       http->version_major,
	       http->version_minor,
	       EZCFG_HTTP_CRLF_STRING);
  if (n < 0) {
    return -1;
  }
  return n;
}

int ezcfg_http_get_status_line_length(struct ezcfg_http *http)
{
  struct ezcfg *ezcfg;
  int n, count;
  unsigned short status_code;
  char *reason_phrase;

  ASSERT(http != NULL);

  ezcfg = http->ezcfg;

  status_code = http->status_code_maps[http->status_code_index].status_code;
  reason_phrase = http->status_code_maps[http->status_code_index].reason_phrase;

  if (status_code == 0) {
    err(ezcfg, "unknown http response status code\n");
    return -1;
  }

  if (reason_phrase == NULL) {
    err(ezcfg, "unknown http response status code reason phrase\n");
    return -1;
  }

  count = 5; /* "HTTP/" */
  n = http->version_major;
  do {
    count++;
    n /= 10;
  } while (n > 0);
  count++; /* "." */
  n = http->version_minor;
  do {
    count++;
    n /= 10;
  } while (n > 0);
  count++; /* SP */
  count += 3; /* status_code */
  count ++; /* SP */
  n = strlen(reason_phrase);
  count += n;
  n = strlen(EZCFG_HTTP_CRLF_STRING);
  count += n;

  return count;
}

int ezcfg_http_write_status_line(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;
  int n;
  unsigned short status_code;
  char *reason_phrase;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  status_code = http->status_code_maps[http->status_code_index].status_code;
  reason_phrase = http->status_code_maps[http->status_code_index].reason_phrase;

  if (status_code == 0) {
    err(ezcfg, "unknown http response status code\n");
    return -1;
  }

  if (reason_phrase == NULL) {
    err(ezcfg, "unknown http response status code reason phrase\n");
    return -1;
  }

  n = snprintf(buf, len, "HTTP/%d.%d %3d %s%s",
	       http->version_major,
	       http->version_minor,
	       status_code,
	       reason_phrase,
	       EZCFG_HTTP_CRLF_STRING);
  if (n < 0) {
    return -1;
  }
  return n;
}

int ezcfg_http_get_start_line_length(struct ezcfg_http *http)
{
  struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  ezcfg = http->ezcfg;

  if (http->state == HTTP_REQUEST) {
    return ezcfg_http_get_request_line_length(http);
  }
  else if (http->state == HTTP_RESPONSE) {
    return ezcfg_http_get_status_line_length(http);
  }
  else {
    err(ezcfg, "unknown http state\n");
    return -1;
  }
}

int ezcfg_http_write_start_line(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  if (http->state == HTTP_REQUEST) {
    return ezcfg_http_write_request_line(http, buf, len);
  }
  else if (http->state == HTTP_RESPONSE) {
    return ezcfg_http_write_status_line(http, buf, len);
  }
  else {
    err(ezcfg, "unknown http state\n");
    return -1;
  }
}

int ezcfg_http_get_crlf_length(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

  return strlen(EZCFG_HTTP_CRLF_STRING);
}

int ezcfg_http_write_crlf(struct ezcfg_http *http, char *buf, int len)
{
  //struct ezcfg *ezcfg;
  int n;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  //ezcfg = http->ezcfg;

  n = snprintf(buf, len, EZCFG_HTTP_CRLF_STRING);
  if (n < 2) {
    return -1;
  }
  return n;
}

int ezcfg_http_get_headers_length(struct ezcfg_http *http)
{
  struct ezcfg *ezcfg;
  struct http_header *h;
  int n, count;

  ASSERT(http != NULL);

  ezcfg = http->ezcfg;

  if (http->method_index == 0) {
    err(ezcfg, "unknown http method\n");
    return -1;
  }

  h = http->header_head;
  count = 0;
  while (h != NULL) {
    n = strlen(h->name);
    count += n;
    count++; /* for ":" */
    n = (h->value ? strlen(h->value) : 0);
    count += n;
    n = strlen(EZCFG_HTTP_CRLF_STRING);
    count += n;
    h = h->next;
  }
  return count;
}

int ezcfg_http_write_headers(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;
  struct http_header *h;
  int n, count;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  if (http->method_index == 0) {
    err(ezcfg, "unknown http method\n");
    return -1;
  }

  h = http->header_head;
  count = 0;
  while (h != NULL) {
    n = snprintf(buf+count, len-count, "%s:%s%s",
		 h->name,
		 h->value ? h->value : "",
		 EZCFG_HTTP_CRLF_STRING);
    if (n < 0) {
      return -1;
    }
    count += n;
    h = h->next;
  }
  return count;
}

bool ezcfg_http_add_header(struct ezcfg_http *http, char *name, char *value)
{
  struct ezcfg *ezcfg;
  struct http_header *h;

  ASSERT(http != NULL);
  ASSERT(name != NULL);
  ASSERT(value != NULL);

  ezcfg = http->ezcfg;

  h = calloc(1, sizeof(struct http_header));
  if (h == NULL) {
    err(ezcfg, "HTTP add header no memory.\n");
    return false;
  }
  memset(h, 0, sizeof(struct http_header));

  h->name = find_known_header_name(http, name);
  if (h->name != NULL) {
    /* A known header */
    h->is_known_header = true;
  } else {
    /* treat it as an extension-header */
    h->is_known_header = false;
    h->name = strdup(name);
    if (h->name == NULL) {
      err(ezcfg, "HTTP add header no memory 2.\n");
      free(h);
      return false;
    }
  }

  if (value[0] != '\0') {
    h->value = strdup(value);
    if (h->value == NULL) {
      err(ezcfg, "HTTP add header no memory 3.\n");
      if (h->is_known_header == false) {
	free(h->name);
      }
      free(h);
      return false;
    }
  }

  /* add to header list tail */
  if (http->header_tail == NULL) {
    http->header_tail = h;
    http->header_head = h;
    h->next = NULL;
  } else {
    http->header_tail->next = h;
    http->header_tail = h;
    h->next = NULL;
  }
  return true;
}

int ezcfg_http_write_message_body(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

  if (len < http->message_body_len) {
    err(ezcfg, "buf length is too small\n");
    return -1;
  }

  if (http->message_body != NULL) {
    memcpy(buf, http->message_body, http->message_body_len);
  }

  return http->message_body_len;
}

int ezcfg_http_get_message_length(struct ezcfg_http *http)
{
  //struct ezcfg *ezcfg;

  int n, count;

  ASSERT(http != NULL);

  //ezcfg = http->ezcfg;

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

int ezcfg_http_write_message(struct ezcfg_http *http, char *buf, int len)
{
  struct ezcfg *ezcfg;

  char *p;
  int n;

  ASSERT(http != NULL);
  ASSERT(buf != NULL);
  ASSERT(len > 0);

  ezcfg = http->ezcfg;

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

bool ezcfg_http_parse_auth(struct ezcfg_http *http, struct ezcfg_auth *auth)
{
  //struct ezcfg *ezcfg;
  char buf[1024];
  char *p;
  bool ret;

  ASSERT(http != NULL);
  ASSERT(auth != NULL);

  //ezcfg = http->ezcfg;

  p = get_http_header_value(http, EZCFG_HTTP_HEADER_AUTHORIZATION);
  if (p == NULL) {
    /* no authorization header, treat it as false */
    return false;
  }

  if (strncmp(p, "Basic ", 6) == 0) {
    p += 6; /* skip "Basic " */
    if (ezcfg_util_base64_decode((unsigned char *)p, (unsigned char *)buf, strlen(p), sizeof(buf)) < 0)
      return false;
    p = strchr(buf, ':');
    if (p == NULL)
      return false;

    /* set auth type */
    ret = ezcfg_auth_set_type(auth, EZCFG_AUTH_TYPE_HTTP_BASIC_STRING);
    if (ret == false)
      return false;

    /* set auth user */
    *p = '\0';
    ret = ezcfg_auth_set_user(auth, buf);
    if (ret == false)
      return false;

    /* set auth secret */
    p++;
    ret = ezcfg_auth_set_secret(auth, p);
    if (ret == false)
      return false;

    return true;
  }

  return false;
}

bool ezcfg_http_parse_post_data(struct ezcfg_http *http, struct ezcfg_nv_linked_list *list)
{
  //struct ezcfg *ezcfg;
  char *p, *q;
  char *end, *eq;
  int ret;

  ASSERT(http != NULL);
  ASSERT(list != NULL);

  //ezcfg = http->ezcfg;

  p = http->message_body;

  while(p != NULL) {
    end = strchr(p, '&');
    if (end != NULL) {
      *end = '\0';

      eq = strchr(p, '=');
      if (eq == NULL) {
	/* restore to '&' */
	*end = '&';
	return false;
      }
      *eq = '\0';
      q = eq+1;

      ret = ezcfg_nv_linked_list_insert(list, p, q);

      /* restore to '=' */
      *eq = '=';
      /* restore to '&' */
      *end = '&';
      if (ret != EZCFG_RET_OK) {
	return false;
      }
      p = end+1;
    }
    else {
      eq = strchr(p, '=');
      if (eq == NULL) {
	return false;
      }
      *eq = '\0';
      q = eq+1;

      ret = ezcfg_nv_linked_list_insert(list, p, q);

      /* restore to '=' */
      *eq = '=';
      if (ret != EZCFG_RET_OK) {
	return false;
      }
      p = NULL;
    }
  }

  return true;
}

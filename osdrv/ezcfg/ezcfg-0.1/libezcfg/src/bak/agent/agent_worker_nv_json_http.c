/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_worker_nv_json_http.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Modify it from agent/agent_worker_nv_json_http.c
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
#include "ezcfg-nv_json_http.h"

static bool nv_json_http_error_handler(struct ezcfg_agent_worker *worker)
{
  return false;
}

static void send_nv_json_http_error(struct ezcfg_agent_worker *worker, int status,
                            const char *reason, const char *fmt, ...)
{
  char buf[EZCFG_BUFFER_SIZE];
  va_list ap;
  int len;
  bool handled;

  //ezcfg_http_set_status_code(worker->proto_data, status);
  handled = nv_json_http_error_handler(worker);

  if (handled == false) {
    buf[0] = '\0';
    len = 0;

    /* Errors 1xx, 204 and 304 MUST NOT send a body */
    if ((status > 199) && (status != 204) && (status != 304)) {
      len = snprintf(buf, sizeof(buf),
                     "Error %d: %s\n", status, reason);
      va_start(ap, fmt);
      len += vsnprintf(buf + len, sizeof(buf) - len, fmt, ap);
      va_end(ap);
      ezcfg_agent_worker_set_num_bytes_sent(worker, len);
    }
    ezcfg_agent_worker_printf(worker,
		              "HTTP/1.1 %d %s\r\n"
		              "Content-Type: text/plain\r\n"
		              "Content-Length: %d\r\n"
		              "Connection: close\r\n"
		              "\r\n%s", status, reason, len, buf);
  }
}

static bool is_nv_json_http_request(struct ezcfg_http *http)
{
  char *uri = ezcfg_http_get_request_uri(http);

  if (uri == NULL) {
    return false;
  }

  if ((strcmp(uri, EZCFG_NV_JSON_HTTP_URI) == 0) &&
      (ezcfg_http_request_method_cmp(http, EZCFG_HTTP_METHOD_POST) == 0)) {
    return true;
  }
  else {
    return false;
  }
}

static void handle_nv_json_http_request(struct ezcfg_agent_worker *worker)
{
  struct ezcfg *ezcfg;
  struct ezcfg_nv_json_http *njh;
  struct ezcfg_http *http;
  char *msg = NULL;
  int msg_len;

  ASSERT(worker != NULL);

  njh = (struct ezcfg_nv_json_http *)ezcfg_agent_worker_get_proto_data(worker);
  ASSERT(njh != NULL);

  ezcfg = ezcfg_agent_worker_get_ezcfg(worker);
  http = ezcfg_nv_json_http_get_http(njh);

  if (is_nv_json_http_request(http) == true) {
    if (ezcfg_nv_json_http_handle_request(njh) != EZCFG_RET_OK) {
      /* clean http structure info */
      ezcfg_http_reset_attributes(http);
      ezcfg_http_set_status_code(http, 400);
      ezcfg_http_set_state_response(http);

      /* build JSON/HTTP error response */
      msg_len = ezcfg_nv_json_http_get_message_length(njh);
      if (msg_len < 0) {
        err(ezcfg, "ezcfg_nv_json_http_get_message_length error.\n");
        goto exit;
      }
      msg_len++; /* one more for '\0' */
      msg = (char *)malloc(msg_len);
      if (msg == NULL) {
        err(ezcfg, "malloc msg error.\n");
        goto exit;
      }
      memset(msg, 0, msg_len);
      msg_len = ezcfg_nv_json_http_write_message(njh, msg, msg_len);
      ezcfg_agent_worker_write(worker, msg, msg_len);
      goto exit;
    }
    else {
      /* build JSON/HTTP binding response */
      msg_len = ezcfg_nv_json_http_get_message_length(njh);
      if (msg_len < 0) {
        err(ezcfg, "ezcfg_nv_json_http_get_message_length error.\n");
        goto exit;
      }
      msg_len++; /* one more for '\0' */
      msg = (char *)malloc(msg_len);
      if (msg == NULL) {
        err(ezcfg, "malloc msg error.\n");
        goto exit;
      }
      memset(msg, 0, msg_len);
      msg_len = ezcfg_nv_json_http_write_message(njh, msg, msg_len);
      ezcfg_agent_worker_write(worker, msg, msg_len);
      goto exit;
    }
  }
  else {
    err(ezcfg, "unknown nv_json_http request.\n");

    /* clean http structure info */
    ezcfg_http_reset_attributes(http);
    ezcfg_http_set_status_code(http, 400);
    ezcfg_http_set_state_response(http);

    /* build nvram JSON/HTTP binding error response */
    msg_len = ezcfg_nv_json_http_get_message_length(njh);
    if (msg_len < 0) {
      err(ezcfg, "ezcfg_nv_json_http_get_message_length error.\n");
      goto exit;
    }
    msg_len++; /* one more for '\0' */
    msg = (char *)malloc(msg_len);
    if (msg == NULL) {
      err(ezcfg, "malloc msg error.\n");
      goto exit;
    }
    memset(msg, 0, msg_len);
    msg_len = ezcfg_nv_json_http_write_message(njh, msg, msg_len);
    ezcfg_agent_worker_write(worker, msg, msg_len);
    goto exit;
  }
 exit:
  if (msg != NULL)
    free(msg);
}

void ezcfg_agent_worker_process_nv_json_http_new_connection(struct ezcfg_agent_worker *worker)
{
  int header_len, nread;
  char *buf;
  int buf_len;
  struct ezcfg *ezcfg;
  struct ezcfg_nv_json_http *njh;
  struct ezcfg_http *http;

  ASSERT(worker != NULL);

  njh = (struct ezcfg_nv_json_http *)ezcfg_agent_worker_get_proto_data(worker);
  ASSERT(njh != NULL);

  ezcfg = ezcfg_agent_worker_get_ezcfg(worker);
  http = ezcfg_nv_json_http_get_http(njh);
  buf_len = EZCFG_JSON_HTTP_CHUNK_SIZE ;

  buf = malloc(buf_len+1); /* +1 for \0 */
  if (buf == NULL) {
    err(ezcfg, "not enough memory for processing JSON/HTTP new connection\n");
    return;
  }
  memset(buf, 0, buf_len+1);
  nread = 0;
  header_len = ezcfg_socket_read_http_header(ezcfg_agent_worker_get_socket(worker), http, buf, buf_len, &nread);

  ASSERT(nread >= header_len);

  if (header_len <= 0) {
    err(ezcfg, "request error\n");
    free(buf);
    return; /* Request is too large or format is not correct */
  }

  /* 0-terminate the request: parse http request uses sscanf
   * !!! never, be careful not mangle the "\r\n\r\n" string!!!
   */
  //buf[header_len - 1] = '\0';
  ezcfg_http_set_state_request(http);
  if (ezcfg_nv_json_http_parse_header(njh, buf, header_len) == true) {
    unsigned short major, minor;
    int ret;
    major = ezcfg_nv_json_http_get_http_version_major(njh);
    minor = ezcfg_nv_json_http_get_http_version_minor(njh);
    if ((major != 1) || (minor != 1)) {
      send_nv_json_http_error(worker, 505,
                             "JSON/HTTP binding version not supported",
                             "%s", "Weird HTTP version");
      goto exit;
    }
    ret = ezcfg_socket_read_http_content(ezcfg_agent_worker_get_socket(worker), http, &buf, header_len, &buf_len, &nread);
    if (ret != EZCFG_RET_OK) {
      /* Do not put garbage in the access log */
      send_nv_json_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
      goto exit;
    }
    if (nread > header_len) {
      ezcfg_nv_json_http_set_message_body(njh, buf + header_len, nread - header_len);
      ezcfg_nv_json_http_parse_message_body(njh);
    }
    ezcfg_agent_worker_set_birth_time(worker, time(NULL));
    handle_nv_json_http_request(worker);
  } else {
    /* Do not put garbage in the access log */
    send_nv_json_http_error(worker, 400, "Bad Request", "Can not parse request: %.*s", nread, buf);
  }

 exit:
  /* release buf memory */
  free(buf);
}

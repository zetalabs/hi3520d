/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : basic/socket/socket_http.c
 *
 * Description  : interface to configurate ezbox information
 *                handle HTTP/socket operations
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-30   0.1       Write it from scratch
 * 2015-06-13   0.2       Move it to basic/socket/socket_http.c
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


/* Check whether full HTTP header is buffered. Return:
 *   -1  if HTTP header is malformed
 *    0  if HTTP header is not yet fully buffered
 *   >0  actual HTTP header length, including last \r\n\r\n
 */
static int get_http_header_len(const char *buf, size_t buflen)
{
  const char *s, *e;
  int len = 0;

  for (s = buf, e = s + buflen - 1; len <= 0 && s < e; s++) {
    /* Control characters are not allowed but >=128 is. */
    if (!isprint(* (unsigned char *) s) && *s != '\r' &&
	*s != '\n' && * (unsigned char *) s < 128) {
      len = -1;
    } else if (s[0] == '\n' && s[1] == '\n') {
      len = (int) (s - buf) + 2;
    } else if (s[0] == '\n' && &s[1] < e &&
	       s[1] == '\r' && s[2] == '\n') {
      len = (int) (s - buf) + 3;
    }
  }

  return len;
}

/**
 * Public functions
 */

/**
 * Keep reading the input into buffer buf, until \r\n\r\n appears in the
 * buffer (which marks the end of HTTP header). Buffer buf may already
 * have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 **/
int ezcfg_socket_read_http_header(struct ezcfg_socket *sp, struct ezcfg_http *http, char *buf, int bufsiz, int *nread)
{
  //struct ezcfg *ezcfg;
  int n, len;

  ASSERT(sp != NULL);
  ASSERT(http != NULL);

  //ezcfg = ezcfg_socket_get_ezcfg(sp);

  len = 0;

  while ((*nread < bufsiz) && (len == 0)) {
    n = ezcfg_socket_read(sp, buf + *nread, bufsiz - *nread, 0);
    if (n <= 0) {
      break;
    } else {
      *nread += n;
      len = get_http_header_len(buf, (size_t) *nread);
    }
  }

  return len;
}

/**
 * Keep reading the input into buffer buf, until reach max buffer size or error.
 * Buffer buf may already have some data. The length of the data is stored in nread.
 * Upon every read operation, increase nread by the number of bytes read.
 **/
int ezcfg_socket_read_http_content(struct ezcfg_socket *sp, struct ezcfg_http *http, char **pbuf, int header_len, int *bufsiz, int *nread)
{
  //struct ezcfg *ezcfg;
  int n;
  char *p, *q, *buf;
  int content_length, buf_len, chunk_size;

  ASSERT(sp != NULL);
  ASSERT(http != NULL);
  ASSERT(pbuf != NULL);

  //ezcfg = ezcfg_socket_get_ezcfg(sp);
  buf_len = *bufsiz;

  if ((p = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_CONTENT_LENGTH)) != NULL) {
    content_length = atoi(p);
    if (content_length > EZCFG_HTTP_MAX_BUFFER_SIZE) {
      return EZCFG_RET_FAIL;
    }
    if ((*nread - header_len) < content_length) {
      /* need to read more data from socket */
      buf_len += (content_length - (*nread - header_len));
      buf = realloc(*pbuf, buf_len);
      if (buf == NULL) {
	return EZCFG_RET_FAIL;
      }
      *pbuf = buf;
      *bufsiz = buf_len;
      while (*nread < buf_len) {
	n = ezcfg_socket_read(sp, buf + *nread, *bufsiz - *nread, 0);
	if (n <= 0) {
	  break;
	}
	else {
	  *nread += n;
	}
      }

      if ((*nread - header_len) < content_length) {
	/* can not read all content */
	return EZCFG_RET_FAIL;
      }
    }
  }
  else if ((p = ezcfg_http_get_header_value(http, EZCFG_HTTP_HEADER_TRANSFER_ENCODING)) != NULL) {
    if (strcmp(p, "chunked") != 0) {
      /* unknown Transfer-Encoding */
      return EZCFG_RET_FAIL;
    }

    do {
      if (buf_len <= *nread) {
	buf_len += EZCFG_BUFFER_SIZE;
      }
      if (buf_len > EZCFG_HTTP_MAX_REQUEST_SIZE) {
	/* too large for the request */
	return EZCFG_RET_FAIL;
      }
      buf = realloc(*pbuf, buf_len);
      if (buf == NULL) {
	return EZCFG_RET_FAIL;
      }
      *pbuf = buf;
      *bufsiz = buf_len;
      while (*nread < *bufsiz) {
	n = ezcfg_socket_read(sp, buf + *nread, *bufsiz - *nread, 0);
	if (n <= 0) {
	  break;
	}
	else {
	  *nread += n;
	}
      }
    } while (*nread == buf_len);

    /* concate chunk data */
    /* 0-terminate the content */
    buf[*nread] = '\0';
    /* point to chunk size */
    p = buf + header_len;
    content_length = 0;
    chunk_size = strtol(p, NULL, 16);
    while((chunk_size > 0) && (chunk_size <= EZCFG_HTTP_CHUNK_SIZE)) {
      q = strstr(p, EZCFG_HTTP_CRLF_STRING);
      if (q == NULL) {
	return EZCFG_RET_FAIL;
      }
      q += strlen(EZCFG_HTTP_CRLF_STRING);
      memmove(buf + header_len + content_length, q, chunk_size);
      content_length += chunk_size;
      p = q + chunk_size + strlen(EZCFG_HTTP_CRLF_STRING);
      chunk_size = strtol(p, NULL, 16);
    }
    if ((chunk_size < 0) || (chunk_size > EZCFG_HTTP_CHUNK_SIZE)) {
      return EZCFG_RET_FAIL;
    }
    /* update nread */
    *nread = header_len + content_length;
  }

  return EZCFG_RET_OK;
}

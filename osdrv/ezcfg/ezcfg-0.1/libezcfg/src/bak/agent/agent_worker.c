/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : agent/agent_worker.c
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2013-08-01   0.1       Modify it from thread/worker.c
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
#include "ezcfg-soap_http.h"

/*
 * ezcfg_agent_worker:
 * Opaque object handling one event source.
 * Multi-threads model - worker part.
 */
struct ezcfg_agent_worker {
  pthread_t thread_id;
  struct ezcfg *ezcfg;
  struct ezcfg_agent_worker *next; /* Linkage */
  struct ezcfg_agent_master *master;
  struct ezcfg_socket *sp;
  unsigned char proto;
  void *proto_data;
  time_t birth_time;
  int64_t num_bytes_sent;
};

#define handle_error_en(en, msg) \
  do { errno = en; perror(msg); } while (0)

static void reset_connection_attributes(struct ezcfg_agent_worker *worker) {
  struct ezcfg *ezcfg;

  ASSERT(worker != NULL);

  ezcfg = worker->ezcfg;

  switch(worker->proto) {
  case EZCFG_PROTO_JSON_HTTP :
    ezcfg_json_http_reset_attributes(worker->proto_data);
    break;
  default :
    err(ezcfg, "unknown protocol\n");
  }

  worker->num_bytes_sent = 0;
}

static void close_connection(struct ezcfg_agent_worker *worker)
{
  ASSERT(worker != NULL);

  ezcfg_socket_close_sock(worker->sp);
}

static void init_protocol_data(struct ezcfg_agent_worker *worker)
{
  struct ezcfg *ezcfg;

  ASSERT(worker != NULL);
  /* proto_data should be empty before init */
  ASSERT(worker->proto_data == NULL);

  ezcfg = worker->ezcfg;

  /* set communication protocol */
  worker->proto = ezcfg_socket_get_proto(worker->sp);

  /* initialize protocol data structure */
  switch(worker->proto) {
  case EZCFG_PROTO_JSON_HTTP :
    worker->proto_data = ezcfg_json_http_new(ezcfg);
    break;
  default :
    info(ezcfg, "unknown protocol\n");
  }
}

static void process_new_connection(struct ezcfg_agent_worker *worker)
{
  struct ezcfg *ezcfg;

  ASSERT(worker != NULL);

  ezcfg = worker->ezcfg;

  reset_connection_attributes(worker);

  /* dispatch protocol handler */
  switch(worker->proto) {
  case EZCFG_PROTO_NV_JSON_HTTP :
    ezcfg_agent_worker_process_nv_json_http_new_connection(worker);
    break;
  default :
    err(ezcfg, "unknown protocol\n");
  }
}
 
static void release_protocol_data(struct ezcfg_agent_worker *worker)
{
  struct ezcfg *ezcfg;

  ASSERT(worker != NULL);

  ezcfg = worker->ezcfg;

  /* release protocol data */
  switch(worker->proto) {
  case EZCFG_PROTO_JSON_HTTP :
    ezcfg_json_http_delete(worker->proto_data);
    worker->proto_data = NULL;
    break;
  default :
    err(ezcfg, "unknown protocol\n");
  }
}

/**
 * Public functions
 **/
void ezcfg_agent_worker_delete(struct ezcfg_agent_worker *worker)
{
  ASSERT(worker != NULL);

  if (worker->sp != NULL) {
    ezcfg_socket_delete(worker->sp);
  }

  free(worker);
}

struct ezcfg_agent_worker *ezcfg_agent_worker_new(struct ezcfg_agent_master *master)
{
  struct ezcfg *ezcfg;
  struct ezcfg_agent_worker *worker = NULL;
  struct ezcfg_socket *sp = NULL;

  ASSERT(master != NULL);
  worker = calloc(1, sizeof(struct ezcfg_agent_worker));
  if (worker == NULL)
    return NULL;

  ezcfg = ezcfg_agent_master_get_ezcfg(master);
  sp = ezcfg_socket_calloc(ezcfg, 1);
  if (sp == NULL) {
    free(worker);
    return NULL;
  }

  memset(worker, 0, sizeof(struct ezcfg_agent_worker));

  worker->ezcfg = ezcfg;
  worker->master = master;
  worker->sp = sp;
  worker->proto = EZCFG_PROTO_UNKNOWN;
  worker->proto_data = NULL;

  return worker;
}

pthread_t *ezcfg_agent_worker_get_p_thread_id(struct ezcfg_agent_worker *worker)
{
  return &(worker->thread_id);
}

struct ezcfg_agent_worker *ezcfg_agent_worker_get_next(struct ezcfg_agent_worker *worker)
{
  ASSERT(worker != NULL);
  return worker->next;
}

bool ezcfg_agent_worker_set_next(struct ezcfg_agent_worker *worker, struct ezcfg_agent_worker *next)
{
  ASSERT(worker != NULL);
  worker->next = next;
  return true;
}

void ezcfg_agent_worker_close_connection(struct ezcfg_agent_worker *worker)
{
  close_connection(worker);
}

void ezcfg_agent_worker_routine(struct ezcfg_agent_worker *worker) 
{
  struct ezcfg_agent_master *master;
  //sigset_t sigset;
  //int s;

  ASSERT(worker != NULL);

  master = worker->master;

  /* Block signal HUP, USR1 */
  /* do it in root thread */
#if 0
  sigemptyset(&sigset);
  sigaddset(&sigset, SIGHUP);
  sigaddset(&sigset, SIGUSR1);
  s = pthread_sigmask(SIG_BLOCK, &sigset, NULL);
  if (s != 0) {
    handle_error_en(s, "pthread_sigmask");
    goto worker_exit;
  }
#endif

  while ((ezcfg_agent_master_is_stop(worker->master) == false) &&
         (ezcfg_agent_master_get_socket(worker->master, worker->sp, EZCFG_WORKER_WAIT_TIME) == true)) {

    /* record start working time */
    worker->birth_time = time(NULL);

    /* initialize protocol data */
    init_protocol_data(worker);

    /* process the connection */
    if (worker->proto_data != NULL) {
      process_new_connection(worker);
    }

    /* close connection */
    close_connection(worker);

    /* release protocol data */
    if (worker->proto_data != NULL) {
      release_protocol_data(worker);
    }
  }

  /* clean worker resource */
  /* do it in ezcfg_agent_master_stop_worker */
  //ezcfg_agent_worker_delete(worker);

  /* Signal master that we're done with connection and exiting */
  ezcfg_agent_master_stop_worker(master, worker);
}

struct ezcfg *ezcfg_agent_worker_get_ezcfg(struct ezcfg_agent_worker *worker)
{
  return worker->ezcfg;
}

struct ezcfg_agent_master *ezcfg_agent_worker_get_master(struct ezcfg_agent_worker *worker)
{
  return worker->master;
}

int ezcfg_agent_worker_printf(struct ezcfg_agent_worker *worker, const char *fmt, ...)
{
  char *buf;
  int buf_len;
  int len;
  int ret;
  va_list ap;

  buf_len = EZCFG_BUFFER_SIZE ;
  buf = (char *)malloc(buf_len);
  if (buf == NULL) {
    return -1;
  }

  va_start(ap, fmt);
  len = vsnprintf(buf, buf_len, fmt, ap);
  va_end(ap);

  ret = ezcfg_socket_write(worker->sp, buf, len, 0);
  free(buf);
  return ret;
}

int ezcfg_agent_worker_write(struct ezcfg_agent_worker *worker, const char *buf, int len)
{
  return ezcfg_socket_write(worker->sp, buf, len, 0);
}

void *ezcfg_agent_worker_get_proto_data(struct ezcfg_agent_worker *worker)
{
  return worker->proto_data;
}

struct ezcfg_socket *ezcfg_agent_worker_get_socket(struct ezcfg_agent_worker *worker)
{
  return worker->sp;
}

bool ezcfg_agent_worker_set_num_bytes_sent(struct ezcfg_agent_worker *worker, int64_t num)
{
  worker->num_bytes_sent = num;
  return true;
}

bool ezcfg_agent_worker_set_birth_time(struct ezcfg_agent_worker *worker, time_t t)
{
  worker->birth_time = t;
  return true;
}

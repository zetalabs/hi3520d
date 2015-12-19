/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_basic_thread.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-10   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_BASIC_THREAD_H_
#define _EZCFG_PRIV_BASIC_THREAD_H_

#include "ezcfg-types.h"

/* basic/thread/thread.c */
struct ezcfg_thread *ezcfg_thread_new(struct ezcfg *ezcfg, char *ns);
int ezcfg_thread_del(struct ezcfg_thread *thread);

int ezcfg_thread_set_start_routine(struct ezcfg_thread *thread, void *(*func)(void *));
int ezcfg_thread_set_arg_del_handler(struct ezcfg_thread *thread, int (*func)(void *));
int ezcfg_thread_set_stop(struct ezcfg_thread *thread, int (*func)(struct ezcfg_thread *));
int ezcfg_thread_start(struct ezcfg_thread *thread);
int ezcfg_thread_stop(struct ezcfg_thread *thread);

#endif /* _EZCFG_PRIV_BASIC_THREAD_H_ */

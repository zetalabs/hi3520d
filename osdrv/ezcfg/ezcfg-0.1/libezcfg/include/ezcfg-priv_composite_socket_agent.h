/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-priv_composite_socket_agent.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-12-20   0.1       Split it from ezcfg-private.h
 * ============================================================================
 */

#ifndef _EZCFG_PRIV_COMPOSITE_SOCKET_AGENT_H_
#define _EZCFG_PRIV_COMPOSITE_SOCKET_AGENT_H_

#include "ezcfg-types.h"
#include "ezcfg-http.h"

/* composite/socket_agent/socket_agent.c */
struct ezcfg_socket_agent;
struct ezcfg_socket_agent *ezcfg_socket_agent_new(struct ezcfg *ezcfg);
int ezcfg_socket_agent_del(struct ezcfg_socket_agent *agent);

int ezcfg_socket_agent_start(struct ezcfg_socket_agent *agent);
int ezcfg_socket_agent_stop(struct ezcfg_socket_agent *agent);

#endif /* _EZCFG_PRIV_COMPOSITE_SOCKET_AGENT_H_ */

/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_socket.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2015 by ezbox-project
 *
 * History      Rev       Description
 * 2015-06-07   0.1       Prepare for new NVRAM model
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_SOCKET_H_
#define _EZCFG_NVRAM_SOCKET_H_

/* ezcfg socket nvram name prefix */
#define EZCFG_NVRAM_PREFIX_SOCKET                "socket."

/* ezcfg socket nvram names */
#define EZCFG_NVRAM_SOCKET_DOMAIN                "domain"
#define EZCFG_NVRAM_SOCKET_TYPE                  "type"
#define EZCFG_NVRAM_SOCKET_PROTOCOL              "protocol"
#define EZCFG_NVRAM_SOCKET_LOCAL_ADDRESS         "local_address"
#define EZCFG_NVRAM_SOCKET_REMOTE_ADDRESS        "remote_address"

#endif

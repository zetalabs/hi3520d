/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/**
 * ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-nvram_defaults.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-16   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_NVRAM_DEFAULTS_H_
#define _EZCFG_NVRAM_DEFAULTS_H_


#define GLUE2(a, b)      	a ## b
#define GLUE3(a, b, c)   	a ## b ## c
#define GLUE4(a, b, c, d)	a ## b ## c ## d

#define NVRAM_PREFIX(prefix)      	GLUE2(EZCFG_NVRAM_PREFIX_, prefix)
#define NVRAM_ENTITY(prefix, name)	GLUE4(EZCFG_NVRAM_, prefix, _, name)
#define NVRAM_NAME(prefix, name) \
  NVRAM_PREFIX(prefix) NVRAM_ENTITY(prefix, name)

#define SERVICE_OPTION(service, name)	GLUE4(EZCFG_NVRAM_, service, _, name)

#define NVRAM_SERVICE_OPTION(service, name) \
  NVRAM_PREFIX(service) SERVICE_OPTION(service, name)

#include "ezcfg-nvram_meta.h"
#include "ezcfg-nvram_socket.h"
#include "ezcfg-nvram_thread.h"
#include "ezcfg-nvram_process.h"
#include "ezcfg-nvram_sys.h"
#if 0
#include "ezcfg-env.h"
#include "ezcfg-ezcfg.h"
#include "ezcfg-loopback.h"
#include "ezcfg-eth_lan.h"
#include "ezcfg-wifi_lan.h"
#include "ezcfg-br_lan.h"
#include "ezcfg-lan.h"
#include "ezcfg-eth_wan.h"
#include "ezcfg-adsl_wan.h"
#include "ezcfg-vdsl_wan.h"
#include "ezcfg-wifi_wan.h"
#include "ezcfg-wan.h"
#include "ezcfg-rc.h"
#include "ezcfg-ui.h"
#include "ezcfg-dnsmasq.h"
#include "ezcfg-iptables.h"
#include "ezcfg-wpa_supplicant.h"
#include "ezcfg-dhisd.h"
#include "ezcfg-dhid.h"
#include "ezcfg-dmcrypt.h"
#include "ezcfg-nano_x.h"
#include "ezcfg-kdrive.h"
#include "ezcfg-dillo.h"
#include "ezcfg-emc2.h"
#include "ezcfg-lxc.h"
#include "ezcfg-radvd.h"
#include "ezcfg-upnp.h"
#include "ezcfg-ssl.h"
#include "ezcfg-openssl.h"
#include "ezcfg-ppp.h"
#include "ezcfg-pppoe_server.h"
#include "ezcfg-rp_pppoe.h"
#include "ezcfg-freeradius2_server.h"
#endif

#endif

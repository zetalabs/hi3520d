/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-lan.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-18   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_LAN_H_
#define _EZCFG_LAN_H_

/* ezcfg nvram name prefix */
#define EZCFG_LAN_NVRAM_PREFIX            "lan."

/* ezcfg lan control names */
/* LAN H/W parameters */
#define EZCFG_LAN_IFNAME                 "ifname"
#define EZCFG_LAN_IFNAMES                "ifnames"
#define EZCFG_LAN_HWNAME                 "hwname"
#define EZCFG_LAN_HWADDR                 "hwaddr"
#define EZCFG_LAN_PHYMODE                "phymode"

/* LAN TCP/IP parameters */
#define EZCFG_LAN_IPADDR                 "ipaddr"
#define EZCFG_LAN_NETMASK                "netmask"
#define EZCFG_LAN_GATEWAY                "gateway"
#define EZCFG_LAN_WINS                   "wins"
#define EZCFG_LAN_DOMAIN                 "domain"
#define EZCFG_LAN_LEASE                  "lease"
#define EZCFG_LAN_STP_ENABLE             "stp_enable"

/* default values */
#define EZCFG_LAN_DEFAULT_IPADDR         "192.168.1.1"
#define EZCFG_LAN_DEFAULT_NETMASK        "255.255.255.0"
#define EZCFG_LAN_DEFAULT_GATEWAY        "0.0.0.0"

#endif

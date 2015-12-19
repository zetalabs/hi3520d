/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-iptables.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2012-01-12   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_IPTABLES_H_
#define _EZCFG_IPTABLES_H_

/* ezcfg nvram name prefix */
#define EZCFG_IPTABLES_NVRAM_PREFIX             "ipt."

/* ezcfg iptables config names */
#define EZCFG_IPTABLES_FIREWALL_ENABLE      "firewall_enable"
#define EZCFG_IPTABLES_NAT_ENABLE           "nat_enable"
#define EZCFG_IPTABLES_DMZ_ENABLE           "dmz_enable"

/* ezcfg DMZ config names */
#define EZCFG_IPTABLES_DMZ_DST_IPADDR       "dmz_dst_ipaddr"

#endif

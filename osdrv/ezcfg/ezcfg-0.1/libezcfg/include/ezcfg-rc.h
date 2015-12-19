/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-rc.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-18   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_RC_H_
#define _EZCFG_RC_H_

/* ezcfg nvram name prefix */
#define EZCFG_RC_NVRAM_PREFIX            "rc."

/* ezcfg rc control names */
#define EZCFG_RC_SYSLOGD_ENABLE          "syslogd_enable"
#define EZCFG_RC_KLOGD_ENABLE            "klogd_enable"

#define EZCFG_RC_TELNETD_ENABLE          "telnetd_enable"
#define EZCFG_RC_TELNETD_BINDING         "telnetd_binding"

#define EZCFG_RC_DNSMASQ_ENABLE          "dnsmasq_enable"
#define EZCFG_RC_DNSMASQ_BINDING         "dnsmasq_binding"

#define EZCFG_RC_IPTABLES_ENABLE         "iptables_enable"
#define EZCFG_RC_IPTABLES_BINDING        "iptables_binding"

#define EZCFG_RC_OPENSSL_ENABLE          "openssl_enable"

#define EZCFG_RC_WPA_SUPPLICANT_ENABLE   "wpa_supplicant_enable"

#define EZCFG_RC_DHISD_ENABLE            "dhisd_enable"

#define EZCFG_RC_DHID_ENABLE             "dhid_enable"

#define EZCFG_RC_NANO_X_ENABLE           "nano-X_enable"

#define EZCFG_RC_KDRIVE_ENABLE           "kdrive_enable"

#define EZCFG_RC_DILLO_ENABLE            "dillo_enable"

#define EZCFG_RC_EMC2_ENABLE             "emc2_enable"

#define EZCFG_RC_COOVA_CHILLI_ENABLE     "coova_chilli_enable"

#define EZCFG_RC_LXC_ENABLE              "lxc_enable"

#define EZCFG_RC_RADVD_ENABLE            "radvd_enable"

#define EZCFG_RC_EZCTP_ENABLE            "ezctp_enable"

#define EZCFG_RC_PPPOE_SERVER_ENABLE     "pppoe_server_enable"
#define EZCFG_RC_PPPOE_SERVER_BINDING    "pppoe_server_binding"

#define EZCFG_RC_PPP_RP_PPPOE_ENABLE     "ppp_rp_pppoe_enable"
#define EZCFG_RC_PPP_RP_PPPOE_BINDING    "ppp_rp_pppoe_binding"

#define EZCFG_RC_FREERADIUS2_SERVER_ENABLE      "freeradius2_server_enable"
#define EZCFG_RC_FREERADIUS2_SERVER_BINDING     "freeradius2_server_binding"

#endif

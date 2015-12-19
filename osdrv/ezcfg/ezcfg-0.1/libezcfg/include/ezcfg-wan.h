/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-wan.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-18   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_WAN_H_
#define _EZCFG_WAN_H_

/* ezcfg nvram name prefix */
#define EZCFG_WAN_NVRAM_PREFIX            "wan."

/* ezcfg wan control names */
/* WAN H/W parameters */
#define EZCFG_WAN_IFNAME                 "ifname"
#define EZCFG_WAN_IFNAMES                "ifnames"
#define EZCFG_WAN_HWNAME                 "hwname"
#define EZCFG_WAN_HWADDR                 "hwaddr"
#define EZCFG_WAN_PHYMODE                "phymode"
#define EZCFG_WAN_MACCLONE_ENABLE        "macclone_enable"
#define EZCFG_WAN_CLONE_HWADDR           "clone_hwaddr"

/* WAN TCP/IP parameters */
#define EZCFG_WAN_TYPE                   "type"
#define EZCFG_WAN_RELEASE                "release"
#define EZCFG_WAN_IPADDR                 "ipaddr"
#define EZCFG_WAN_NETMASK                "netmask"
#define EZCFG_WAN_GATEWAY                "gateway"
#define EZCFG_WAN_DNS                    "dns"
#define EZCFG_WAN_DNS1                   "dns1"
#define EZCFG_WAN_DNS2                   "dns2"
#define EZCFG_WAN_DNS3                   "dns3"
#define EZCFG_WAN_WINS                   "wins"
#define EZCFG_WAN_HOSTNAME               "hostname"
#define EZCFG_WAN_DOMAIN                 "domain"
#define EZCFG_WAN_LEASE                  "lease"
#define EZCFG_WAN_MTU_ENABLE             "mtu_enable"
#define EZCFG_WAN_MTU                    "mtu"

/* WAN static IP address parameters */
#define EZCFG_WAN_STATIC_IFNAME          "static_ifname"
#define EZCFG_WAN_STATIC_IPADDR          "static_ipaddr"
#define EZCFG_WAN_STATIC_NETMASK         "static_netmask"
#define EZCFG_WAN_STATIC_GATEWAY         "static_gateway"
#define EZCFG_WAN_STATIC_DNS             "static_dns"
#define EZCFG_WAN_STATIC_DNS1            "static_dns1"
#define EZCFG_WAN_STATIC_DNS2            "static_dns2"
#define EZCFG_WAN_STATIC_DNS3            "static_dns3"

/* WAN DHCP client parameters */
#define EZCFG_WAN_DHCP_IFNAME            "dhcp_ifname"
#define EZCFG_WAN_DHCP_GATEWAY_ENABLE    "dhcp_gateway_enable"
#define EZCFG_WAN_DHCP_GATEWAY_MAC       "dhcp_gateway_mac"
#define EZCFG_WAN_DHCP_WINS              "dhcp_wins"
#define EZCFG_WAN_DHCP_LEASE             "dhcp_lease"

/* WAN PPPoE parameters */
#define EZCFG_WAN_PPPOE_DEVICE_NAME      "pppoe_device_name"
#define EZCFG_WAN_PPPOE_IFNAME           "pppoe_ifname"
#define EZCFG_WAN_PPPOE_KEEP_ENABLE      "pppoe_keep_enable"
#define EZCFG_WAN_PPPOE_PPP_OPTIONS_FILE "pppoe_ppp_options_file"
#define EZCFG_WAN_PPPOE_PPP_USERNAME     "pppoe_ppp_username"
#define EZCFG_WAN_PPPOE_PPP_PASSWD       "pppoe_ppp_passwd"
#define EZCFG_WAN_PPPOE_PPP_IDLE         "pppoe_ppp_idle"
#define EZCFG_WAN_PPPOE_PPP_LCP_ECHO_INTERVAL   "pppoe_ppp_lcp_echo_interval"
#define EZCFG_WAN_PPPOE_PPP_LCP_ECHO_FAILURE    "pppoe_ppp_lcp_echo_failure"
#define EZCFG_WAN_PPPOE_MRU              "pppoe_mru"
#define EZCFG_WAN_PPPOE_MTU              "pppoe_mtu"
#define EZCFG_WAN_PPPOE_SERVICE          "pppoe_service"
#define EZCFG_WAN_PPPOE_ACNAME           "pppoe_acname"

/* WAN PPTP parameters */
#define EZCFG_WAN_PPTP_IFNAME            "pptp_ifname"
#define EZCFG_WAN_PPTP_IPADDR            "pptp_ipaddr"
#define EZCFG_WAN_PPTP_NETMASK           "pptp_netmask"
#define EZCFG_WAN_PPTP_GATEWAY           "pptp_gateway"
#define EZCFG_WAN_PPTP_DNS               "pptp_dns"
#define EZCFG_WAN_PPTP_DNS1              "pptp_dns1"
#define EZCFG_WAN_PPTP_DNS2              "pptp_dns2"
#define EZCFG_WAN_PPTP_DNS3              "pptp_dns3"
#define EZCFG_WAN_PPTP_SERVER_IPADDR     "pptp_server_ipaddr"
#define EZCFG_WAN_PPTP_KEEP_ENABLE       "pptp_keep_enable"
#define EZCFG_WAN_PPTP_PPP_USERNAME             "pptp_ppp_username"
#define EZCFG_WAN_PPTP_PPP_PASSWD               "pptp_ppp_passwd"
#define EZCFG_WAN_PPTP_PPP_IDLE                 "pptp_ppp_idle"
#define EZCFG_WAN_PPTP_PPP_LCP_ECHO_INTERVAL    "pptp_ppp_lcp_echo_interval"
#define EZCFG_WAN_PPTP_PPP_LCP_ECHO_FAILURE     "pptp_ppp_lcp_echo_failure"
#define EZCFG_WAN_PPTP_MRU               "pptp_mru"
#define EZCFG_WAN_PPTP_MTU               "pptp_mtu"

/* WAN L2TP parameters */
#define EZCFG_WAN_L2TP_IFNAME            "l2tp_ifname"
#define EZCFG_WAN_L2TP_SERVER_IPADDR     "l2tp_server_ipaddr"
#define EZCFG_WAN_L2TP_KEEP_ENABLE       "l2tp_keep_enable"
#define EZCFG_WAN_L2TP_PPP_USERNAME             "l2tp_ppp_username"
#define EZCFG_WAN_L2TP_PPP_PASSWD               "l2tp_ppp_passwd"
#define EZCFG_WAN_L2TP_PPP_IDLE                 "l2tp_ppp_idle"
#define EZCFG_WAN_L2TP_PPP_LCP_ECHO_INTERVAL    "l2tp_ppp_lcp_echo_interval"
#define EZCFG_WAN_L2TP_PPP_LCP_ECHO_FAILURE     "l2tp_ppp_lcp_echo_failure"
#define EZCFG_WAN_L2TP_MRU               "l2tp_mru"
#define EZCFG_WAN_L2TP_MTU               "l2tp_mtu"

#endif

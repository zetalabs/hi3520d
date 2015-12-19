/* ============================================================================
 * Project Name : ezbox configuration utilities
 * File Name    : ezcfg-dnsmasq.h
 *
 * Description  : interface to configurate ezbox information
 *
 * Copyright (C) 2008-2014 by ezbox-project
 *
 * History      Rev       Description
 * 2010-11-17   0.1       Write it from scratch
 * ============================================================================
 */

#ifndef _EZCFG_DNSMASQ_H_
#define _EZCFG_DNSMASQ_H_

/* ezcfg nvram name prefix */
#define EZCFG_DNSMASQ_NVRAM_PREFIX        "dnsmasq."

/* ezcfg dnsmasq control names */
#define EZCFG_DNSMASQ_DOMAIN_NEEDED       "domain-needed"
#define EZCFG_DNSMASQ_BOGUS_PRIV          "bogus-priv"
#define EZCFG_DNSMASQ_FILTERWIN2K         "filterwin2k"
#define EZCFG_DNSMASQ_RESOLV_FILE         "resolv-file"
#define EZCFG_DNSMASQ_STRICT_ORDER        "strict-order"
#define EZCFG_DNSMASQ_NO_RESOLV           "no-resolv"
#define EZCFG_DNSMASQ_NO_POLL             "no-poll"
#define EZCFG_DNSMASQ_SERVER              "server"
#define EZCFG_DNSMASQ_LOCAL               "local"
#define EZCFG_DNSMASQ_ADDRESS             "address"
#define EZCFG_DNSMASQ_USER                "user"
#define EZCFG_DNSMASQ_GROUP               "group"
#define EZCFG_DNSMASQ_INTERFACE           "interface"
#define EZCFG_DNSMASQ_EXCEPT_INTERFACE    "except-interface"
#define EZCFG_DNSMASQ_LISTEN_ADDRESS      "listen-address"
#define EZCFG_DNSMASQ_NO_DHCP_INTERFACE   "no-dhcp-interface"
#define EZCFG_DNSMASQ_BIND_INTERFACES     "bind-interfaces"
#define EZCFG_DNSMASQ_NO_HOSTS            "no-hosts"
#define EZCFG_DNSMASQ_ADDN_HOSTS          "addn-hosts"
#define EZCFG_DNSMASQ_EXPAND_HOSTS        "expand-hosts"
#define EZCFG_DNSMASQ_DOMAIN              "domain"
#define EZCFG_DNSMASQ_DHCP_RANGE          "dhcp-range"
#define EZCFG_DNSMASQ_DHCP_HOST           "dhcp-host"
#define EZCFG_DNSMASQ_DHCP_IGNORE         "dhcp-ignore"
#define EZCFG_DNSMASQ_DHCP_VENDORCLASS    "dhcp-vendorclass"
#define EZCFG_DNSMASQ_DHCP_USERCLASS      "dhcp-userclass"
#define EZCFG_DNSMASQ_DHCP_MAC            "dhcp-mac"
#define EZCFG_DNSMASQ_READ_ETHERS         "read-ethers"
#define EZCFG_DNSMASQ_DHCP_OPTION         "dhcp-option"
#define EZCFG_DNSMASQ_DHCP_OPTION_FORCE   "dhcp-option-force"
#define EZCFG_DNSMASQ_DHCP_BOOT           "dhcp-boot"
#define EZCFG_DNSMASQ_DHCP_MATCH          "dhcp-match"
#define EZCFG_DNSMASQ_PXE_PROMPT          "pxe-prompt"
#define EZCFG_DNSMASQ_PXE_SERVICE         "pxe-service"
#define EZCFG_DNSMASQ_ENABLE_TFTP         "enable-tftp"
#define EZCFG_DNSMASQ_TFTP_ROOT           "tftp-root"
#define EZCFG_DNSMASQ_TFTP_SECURE         "tftp-secure"
#define EZCFG_DNSMASQ_TFTP_NO_BLOCKSIZE   "tftp-no-blocksize"
#define EZCFG_DNSMASQ_DHCP_LEASE_MAX      "dhcp-lease-max"
#define EZCFG_DNSMASQ_DHCP_LEASEFILE      "dhcp-leasefile"
#define EZCFG_DNSMASQ_DHCP_AUTHORITATIVE  "dhcp-authoritative"
#define EZCFG_DNSMASQ_DHCP_SCRIPT         "dhcp-script"
#define EZCFG_DNSMASQ_MIN_PORT            "min-port"
#define EZCFG_DNSMASQ_CACHE_SIZE          "cache-size"
#define EZCFG_DNSMASQ_NO_NEGCACHE         "no-negcache"
#define EZCFG_DNSMASQ_LOCAL_TTL           "local-ttl"
#define EZCFG_DNSMASQ_BOGUS_NXDOMAIN      "bogus-nxdomain"
#define EZCFG_DNSMASQ_ALIAS               "alias"
#define EZCFG_DNSMASQ_MX_HOST             "mx-host"
#define EZCFG_DNSMASQ_MX_TARGET           "mx-target"
#define EZCFG_DNSMASQ_LOCALMX             "localmx"
#define EZCFG_DNSMASQ_SELFMX              "selfmx"
#define EZCFG_DNSMASQ_SRV_HOST            "srv-host"
#define EZCFG_DNSMASQ_PTR_RECORD          "ptr-record"
#define EZCFG_DNSMASQ_TXT_RECORD          "txt-record"
#define EZCFG_DNSMASQ_CNAME               "cname"
#define EZCFG_DNSMASQ_LOG_QUERIES         "log-queries"
#define EZCFG_DNSMASQ_LOG_DHCP            "log-dhcp"
#define EZCFG_DNSMASQ_CONF_FILE           "conf-file"
#define EZCFG_DNSMASQ_CONF_DIR            "conf-dir"

/* Web UI settings */
#define EZCFG_DNSMASQ_DHCPD_ENABLE           "dhcpd_enable"
#define EZCFG_DNSMASQ_DHCPD_START_IPADDR     "dhcpd_start_ipaddr"
#define EZCFG_DNSMASQ_DHCPD_END_IPADDR       "dhcpd_end_ipaddr"
#define EZCFG_DNSMASQ_DHCPD_NETMASK          "dhcpd_netmask"
#define EZCFG_DNSMASQ_DHCPD_LEASE            "dhcpd_lease"
#define EZCFG_DNSMASQ_DHCPD_GATEWAY          "dhcpd_gateway"
#define EZCFG_DNSMASQ_DHCPD_WAN_DNS_ENABLE   "dhcpd_wan_dns_enable"
#define EZCFG_DNSMASQ_DHCPD_DNS              "dhcpd_dns"
#define EZCFG_DNSMASQ_DHCPD_DNS1             "dhcpd_dns1"
#define EZCFG_DNSMASQ_DHCPD_DNS2             "dhcpd_dns2"
#define EZCFG_DNSMASQ_DHCPD_DNS3             "dhcpd_dns3"
#define EZCFG_DNSMASQ_DHCPD_DOMAIN           "dhcpd_domain"
#define EZCFG_DNSMASQ_DHCPD_WINS             "dhcpd_wins"

/* nvram default values */
#define EZCFG_DNSMASQ_DEFAULT_DHCPD_START_IPADDR \
	"192.168.1.100"
#define EZCFG_DNSMASQ_DEFAULT_DHCPD_END_IPADDR \
	"192.168.1.149"
#define EZCFG_DNSMASQ_DEFAULT_DHCPD_LEASE \
	"86400"

#endif

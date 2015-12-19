#ifndef _RC_FUNC_H_
#define _RC_FUNC_H_

#if 0
extern int rc_base_files(int argc, char **argv);
extern int rc_ezcd(int argc, char **argv);
extern int rc_nvram(int argc, char **argv);
extern int rc_ezcm(int argc, char **argv);
extern int rc_preboot(int argc, char **argv);
//extern int rc_hotplug2(int argc, char **argv);
extern int rc_data_storage(int argc, char **argv);
extern int rc_load_modules(int argc, char **argv);
extern int rc_mount_system_fs(int argc, char **argv);
extern int rc_login(int argc, char **argv);
extern int rc_loopback(int argc, char **argv);
extern int rc_netbase(int argc, char **argv);

#if (HAVE_EZBOX_SERVICE_OPENSSL == 1)
extern int rc_openssl(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_SYSLOGD == 1)
extern int rc_syslogd(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_syslogd(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_syslogd(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_KLOGD == 1)
extern int rc_klogd(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_klogd(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_klogd(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_DMCRYPT_DATA_PARTITION == 1)
extern int rc_dmcrypt_data_partition(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_IPTABLES == 1)
extern int rc_iptables(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_iptables(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_iptables(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_PPP == 1)
extern int rc_ppp_pap_secrets(int argc, char **argv);
extern int rc_ppp_chap_secrets(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_PPP_RP_PPPOE == 1)
extern int rc_ppp_rp_pppoe(int argc, char **argv);
#endif

#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan(int argc, char **argv);
extern int rc_lan_services(int argc, char **argv);
#endif

#if (HAVE_EZBOX_ETH_LAN_NIC == 1)
extern int rc_eth_lan_if(int argc, char **argv);
#endif

#if (HAVE_EZBOX_WIFI_LAN_NIC == 1)
extern int rc_wifi_lan_if(int argc, char **argv);
#if (HAVE_EZBOX_SERVICE_WPA_SUPPLICANT == 1)
extern int rc_wpa_supplicant(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_BR_LAN_NIC == 1)
extern int rc_br_lan_if(int argc, char **argv);
#endif

#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan(int argc, char **argv);
extern int rc_wan_if(int argc, char **argv);
extern int rc_wan_services(int argc, char **argv);
#endif

#endif

extern int rc_debug(int argc, char **argv);
extern int rc_ldconfig(int argc, char **argv);

extern int rc_ezcfg_httpd(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_ezcfg_httpd(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_ezcfg_httpd(int argc, char **argv);
#endif

extern int rc_mdev(int argc, char **argv);
extern int rc_data_partition(int argc, char **argv);
extern int rc_init(int argc, char **argv);
extern int rc_system(int argc, char **argv);

#if (HAVE_EZBOX_SERVICE_TELNETD == 1)
extern int rc_telnetd(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_telnetd(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_telnetd(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_DNSMASQ == 1)
extern int rc_dnsmasq(int argc, char **argv);
#if (HAVE_EZBOX_LAN_NIC == 1)
extern int rc_lan_dnsmasq(int argc, char **argv);
#endif
#if (HAVE_EZBOX_WAN_NIC == 1)
extern int rc_wan_dnsmasq(int argc, char **argv);
#endif
#endif

#if (HAVE_EZBOX_SERVICE_DHID == 1)
extern int rc_dhid(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_DHISD == 1)
extern int rc_dhisd(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_EZCTP == 1)
extern int rc_ezctp(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_NANO_X == 1)
extern int rc_nano_x(int argc, char **argv);
#elif (HAVE_EZBOX_SERVICE_KDRIVE == 1)
extern int rc_kdrive(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_FONTCONFIG == 1)
extern int rc_fontconfig(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_DILLO == 1)
extern int rc_dillo(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_EMC2 == 1)
extern int rc_realtime(int argc, char **argv);
extern int rc_emc2(int argc, char **argv);
extern int rc_emc2_latency_test(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_COOVA_CHILLI == 1)
extern int rc_coova_chilli(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_LXC == 1)
extern int rc_lxc(int argc, char **argv);
#endif

#if (HAVE_EZBOX_SERVICE_PPPOE_SERVER == 1)
extern int rc_pppoe_server(int argc, char **argv);
#endif

#endif

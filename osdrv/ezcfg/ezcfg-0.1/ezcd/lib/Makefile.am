# ------------------------------------------------------------------------------
# Copyright (C) 2008-2013 ezbox project
# ------------------------------------------------------------------------------

EXTRA_DIST = exported_symbols

SUBDIRS = .

AM_CPPFLAGS = \
	-include ../config.h \
	-I../include \
        -DSYSCONFDIR=\""$(sysconfdir)"\" \
        -DDATADIR="$(datadir)" \
        -DEZCFG_DEBUG \
        -Wall -Werror

AM_LDFLAGS = -L. -ldl

libezcfg.so: $(LIBEZCFG_LIB)
	-rm -f libezcfg.so
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

###############################
# libezcd
###############################
LIBEZCD_CURRENT = 1
LIBEZCD_REVISION = 0
LIBEZCD_AGE = 0

libezcdincludedir = $(includedir)/libezcd

libezcdinclude_HEADERS = \
        ../include/utils.h \
        ../include/ezcd.h

lib_LTLIBRARIES = libezcd.la

libezcd_la_DEPENDENCIES = libezcfg.so

libezcd_la_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)

libezcd_la_LDFLAGS = \
        -version-info $(LIBEZCD_CURRENT):$(LIBEZCD_REVISION):$(LIBEZCD_AGE) \
        -export-symbols $(srcdir)/exported_symbols

# utils
libezcd_la_SOURCES = ../src/utils/utils_crc32.c
#libezcd_la_SOURCES += ../src/utils/utils_check_ezcd.c
#libezcd_la_SOURCES += ../src/utils/utils_check_boot_ready.c
#libezcd_la_SOURCES += ../src/utils/utils_check_data_ready.c
#libezcd_la_SOURCES += ../src/utils/utils_check_partition_ready.c
#libezcd_la_SOURCES += ../src/utils/utils_check_service.c
if CONFIG_EZBOX_WAN_NIC
#libezcd_la_SOURCES += ../src/utils/utils_check_wan_interface.c
endif
#libezcd_la_SOURCES += ../src/utils/utils_switch_root.c
#libezcd_la_SOURCES += ../src/utils/utils_nvram_cmp.c
libezcd_la_SOURCES += ../src/utils/utils_file_get_keyword.c
libezcd_la_SOURCES += ../src/utils/utils_get_kernel_init.c
libezcd_la_SOURCES += ../src/utils/utils_get_kernel_modules.c
libezcd_la_SOURCES += ../src/utils/utils_get_kernel_version.c
libezcd_la_SOURCES += ../src/utils/utils_get_mem_size.c
libezcd_la_SOURCES += ../src/utils/utils_handle_kernel_module.c
libezcd_la_SOURCES += ../src/utils/utils_make_dirs.c
libezcd_la_SOURCES += ../src/utils/utils_clean_dirs.c
libezcd_la_SOURCES += ../src/utils/utils_mount_partition.c
libezcd_la_SOURCES += ../src/utils/utils_swap_partition.c
libezcd_la_SOURCES += ../src/utils/utils_get_bootcfg_keyword.c
libezcd_la_SOURCES += ../src/utils/utils_get_device_info.c
#libezcd_la_SOURCES += ../src/utils/utils_sync_cfg_with_nvram.c
#libezcd_la_SOURCES += ../src/utils/utils_sync_nvram_with_cfg.c
#libezcd_la_SOURCES += ../src/utils/utils_netmask_to_prefix.c
#libezcd_la_SOURCES += ../src/utils/utils_get_rc_act_type.c
#libezcd_la_SOURCES += ../src/utils/utils_cleanup_rootfs.c
if CONFIG_EZBOX_WAN_NIC
#libezcd_la_SOURCES += ../src/utils/utils_get_wan_type.c
endif
libezcd_la_SOURCES += ../src/utils/utils_init_ezcfg_api.c
libezcd_la_SOURCES += ../src/utils/utils_file_get_line.c
libezcd_la_SOURCES += ../src/utils/utils_file_print_line.c
libezcd_la_SOURCES += ../src/utils/utils_file_get_content.c
libezcd_la_SOURCES += ../src/utils/utils_find_pid_by_name.c
libezcd_la_SOURCES += ../src/utils/utils_device_open.c
libezcd_la_SOURCES += ../src/utils/utils_parse_args.c
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#libezcd_la_SOURCES += ../src/utils/utils_parse_upnp_task.c
endif
libezcd_la_SOURCES += ../src/utils/utils_read_write.c
libezcd_la_SOURCES += ../src/utils/utils_execute.c
libezcd_la_SOURCES += ../src/utils/utils_mkdir.c
libezcd_la_SOURCES += ../src/utils/utils_system.c
#libezcd_la_SOURCES += ../src/utils/utils_route_delete_default.c
libezcd_la_SOURCES += ../src/utils/utils_udev_trigger_pop.c

# login
#libezcd_la_SOURCES += ../src/pop/pop_etc_passwd.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_group.c

# mdev
#libezcd_la_SOURCES += ../src/pop/pop_etc_mdev_conf.c

# kernel modules
#libezcd_la_SOURCES += ../src/pop/pop_etc_modules.c

# nvram
#libezcd_la_SOURCES += ../src/pop/pop_etc_nvram_conf.c

# ezcd
#libezcd_la_SOURCES += ../src/pop/pop_etc_ezcd_conf.c

# ezcm
#libezcd_la_SOURCES += ../src/pop/pop_etc_ezcm_conf.c

# netbase
#libezcd_la_SOURCES += ../src/pop/pop_etc_hosts.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_network_interfaces.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_protocols.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_mactab.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_resolv_conf.c

# cgroup
if CONFIG_EZBOX_EZCFG_CGROUP
#libezcd_la_SOURCES += ../src/pop/pop_etc_cgconfig_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_cgrules_conf.c
endif

# openssl service
if CONFIG_EZBOX_SERVICE_OPENSSL
#libezcd_la_SOURCES += ../src/pop/pop_etc_ssl_openssl_cnf.c
endif

# iptables service
if CONFIG_EZBOX_SERVICE_IPTABLES
#libezcd_la_SOURCES += ../src/pop/pop_etc_l7_protocols.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_iptables_modules.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_iptables_firewall.c
endif

# init
#libezcd_la_SOURCES += ../src/pop/pop_etc_inittab.c

# base files
#libezcd_la_SOURCES += ../src/pop/pop_etc_banner.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_hostname.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_mtab.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_profile.c

# ezcfg_igrsd
if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
#libezcd_la_SOURCES += ../src/pop/pop_etc_ezcfg_igrsd.c
endif

# ezcfg_upnpd
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#libezcd_la_SOURCES += ../src/pop/pop_etc_ezcfg_upnpd.c
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD_IGD1
#libezcd_la_SOURCES += ../src/pop/pop_etc_ezcfg_upnpd_igd1.c
endif
endif

# network
if CONFIG_EZBOX_SERVICE_WPA_SUPPLICANT
#libezcd_la_SOURCES += ../src/pop/pop_etc_wpa_supplicant_conf.c
endif

# dhcp/dns service
if CONFIG_EZBOX_SERVICE_DNSMASQ
#libezcd_la_SOURCES += ../src/pop/pop_etc_dnsmasq_conf.c
endif

# DHIS client service
if CONFIG_EZBOX_SERVICE_DHID
#libezcd_la_SOURCES += ../src/pop/pop_etc_dhid_conf.c
endif

# DHIS server service
if CONFIG_EZBOX_SERVICE_DHISD
#libezcd_la_SOURCES += ../src/pop/pop_etc_dhisd_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_dhis_db.c
endif

# LUKS/dm-crypt data partition service
if CONFIG_EZBOX_SERVICE_DMCRYPT_DATA_PARTITION
#libezcd_la_SOURCES += ../src/pop/pop_etc_keys_data_partition_key.c
endif

# dillo web browser service
if CONFIG_EZBOX_SERVICE_DILLO
#libezcd_la_SOURCES += ../src/pop/pop_etc_dillo_dillorc.c
endif

# EMC2 Enhanced Machine Controller service
if CONFIG_EZBOX_SERVICE_EMC2
#libezcd_la_SOURCES += ../src/pop/pop_etc_emc2_rtapi_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_emc2_configs.c
endif

# coova-chilli Hotspot Manager service
if CONFIG_EZBOX_SERVICE_COOVA_CHILLI
#libezcd_la_SOURCES += ../src/pop/pop_etc_chilli_conf.c
endif

# LXC Linux Container service
if CONFIG_EZBOX_SERVICE_LXC
#libezcd_la_SOURCES += ../src/pop/pop_etc_lxc_lxc_conf.c
endif

# radvd Linux IPv6 Router Advertisement Daemon service
if CONFIG_EZBOX_SERVICE_RADVD
#libezcd_la_SOURCES += ../src/pop/pop_etc_radvd_conf.c
endif

# PPP service
if CONFIG_EZBOX_SERVICE_PPP
#libezcd_la_SOURCES += ../src/pop/pop_etc_ppp_pap_secrets.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_ppp_chap_secrets.c
endif

# PPPoE server service
if CONFIG_EZBOX_SERVICE_PPPOE_SERVER
#libezcd_la_SOURCES += ../src/pop/pop_etc_ppp_pppoe_server_options.c
endif

# PPP RP-PPPoE service
if CONFIG_EZBOX_SERVICE_PPP_RP_PPPOE
#libezcd_la_SOURCES += ../src/pop/pop_etc_ppp_rp_pppoe_options.c
endif

# FreeRADIUS2 server service
if CONFIG_EZBOX_SERVICE_FREERADIUS2_SERVER
#libezcd_la_SOURCES += ../src/pop/pop_etc_freeradius2_radiusd_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_freeradius2_clients_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_freeradius2_eap_conf.c
#libezcd_la_SOURCES += ../src/pop/pop_etc_freeradius2_dictionary.c
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libezcd.pc

# ------------------------------------------------------------------------------
# Copyright (C) 2008-2013 ezbox project
# ------------------------------------------------------------------------------

SUBDIRS = .

AM_CPPFLAGS = \
	-include ../config.h \
	-I../include \
        -DSYSCONFDIR=\""$(sysconfdir)"\" \
        -DDATADIR="$(datadir)" \
        -DEZCFG_DEBUG \
        -Wall -Werror

AM_LDFLAGS = -L. -ldl

# ezcd depends on libezcfg
# LIBS += -lezcfg
ezcd_DEPENDENCIES = libezcfg.so libezcd.so $(RCSO_SO) $(RCSO_EXE)
ezcd_DEPENDENCIES += $(AGENT_ENV_SO) $(AGENT_ENV_EXE)
if CONFIG_EZBOX_EZCFG_SYSUP
ezcd_DEPENDENCIES += sysup
endif
ezcd_CPPFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) $(LIBEZCFG_INCLUDE) $(LIBEZCFG_API_INCLUDE)
#ezcd_LDADD = $(LDADD) -L../lib/.libs -lezcd
ezcd_LDFLAGS = $(AM_LDFLAGS) -lezcfg -lezcd
ezcd_LDFLAGS += -Wl,-Map,ezcd.map

sbin_PROGRAMS =	ezcd


# basic functions
ezcd_SOURCES = ../src/main.c
ezcd_SOURCES += ../src/agent_env.c
#ezcd_SOURCES += ../src/preinit.c
#ezcd_SOURCES += ../src/init.c
#ezcd_SOURCES += ../src/ezcd.c
#ezcd_SOURCES += ../src/ezcm.c
#ezcd_SOURCES += ../src/nvram.c
#ezcd_SOURCES += ../src/rc.c
ezcd_SOURCES += ../src/eznvc.c
#ezcd_SOURCES += ../src/shelld.c
#if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
#ezcd_SOURCES += ../src/igrs_monitor.c
#endif
#if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#ezcd_SOURCES += ../src/upnp_monitor.c
#endif

#ezcd_SOURCES += ../src/rc/rc_func.c
#ezcd_SOURCES += ../src/rc/rc_debug.c

# ubootenv support
#ezcd_SOURCES += ../src/ubootenv.c

# udhcpc script support
if CONFIG_EZBOX_WAN_NIC
#ezcd_SOURCES += ../src/udhcpc_script.c
endif

# upgrade firmware support
#ezcd_SOURCES += ../src/upfw.c

libezcfg.so: $(LIBEZCFG_LIB)
	-rm -f libezcfg.so
	$(LN_S) $(LIBEZCFG_LIB)/libezcfg.so libezcfg.so

libezcd.so: ../lib/.libs/libezcd.so
	-rm -f libezcd.so
	$(LN_S) ../lib/.libs/libezcd.so libezcd.so

sysup: ../src/sysup.c
	$(CC) -static -o $@ $<

###############################
# rcso
###############################
RCSO_CFLAGS = -Wall -Werror -rdynamic -fPIC
RCSO_CFLAGS += $(ezcd_CPPFLAGS)

RCSO_SRC = rc_agent.c
RCSO_SO = rc_agent.so
RCSO_EXE = rc_agent.exe

#RCSO_SRC += rc_exec.c
#RCSO_SRC += rc_data_partition.c
#RCSO_SRC += rc_env.c
#RCSO_SRC += rc_ezcfg_httpd.c
if CONFIG_EZBOX_SERVICE_EZCFG_IGRSD
#RCSO_SRC += rc_ezcfg_igrsd.c
endif
if CONFIG_EZBOX_SERVICE_EZCFG_UPNPD
#RCSO_SRC += rc_ezcfg_upnpd.c
#RCSO_SRC += rc_upnp_monitor.c
endif
#RCSO_SRC += rc_resolv_conf.c
#RCSO_SRC += rc_init.c
#RCSO_SRC += rc_ldconfig.c
if CONFIG_EZBOX_SERVICE_WPA_SUPPLICANT
#RCSO_SRC += rc_wpa_supplicant.c
endif

# telnet service
if CONFIG_EZBOX_SERVICE_TELNETD
#RCSO_SRC += rc_telnetd.c
endif
# dhcp/dns service
if CONFIG_EZBOX_SERVICE_DNSMASQ
#RCSO_SRC += rc_dnsmasq.c
endif
# DHIS client service
if CONFIG_EZBOX_SERVICE_DHID
#RCSO_SRC += rc_dhid.c
endif
# DHIS server service
if CONFIG_EZBOX_SERVICE_DHISD
#RCSO_SRC += rc_dhisd.c
endif
# ezctp service
if CONFIG_EZBOX_SERVICE_EZCTP
#RCSO_SRC += rc_ezctp.c
endif
# dillo web browser service
if CONFIG_EZBOX_SERVICE_DILLO
#RCSO_SRC += rc_dillo.c
endif
# EMC2 Enhanced Machine Controller service
if CONFIG_EZBOX_SERVICE_EMC2
#RCSO_SRC += rc_realtime.c
#RCSO_SRC += rc_emc2.c
#RCSO_SRC += rc_emc2_latency_test.c
endif
# coova-chilli Hotspot Manager
if CONFIG_EZBOX_SERVICE_COOVA_CHILLI
#RCSO_SRC += rc_coova_chilli.c
endif
# LXC Linux Container service
if CONFIG_EZBOX_SERVICE_LXC
#RCSO_SRC += rc_lxc.c
endif
# PPPoE server service
if CONFIG_EZBOX_SERVICE_PPPOE_SERVER
#RCSO_SRC += rc_pppoe_server.c
endif
# FreeRADIUS2 server service
if CONFIG_EZBOX_SERVICE_FREERADIUS2_SERVER
#RCSO_SRC += rc_freeradius2_server.c
endif

#RCSO_SO = $(subst .c,.so,$(RCSO_SRC))
#RCSO_EXE = $(subst .c,.exe,$(RCSO_SRC))

###############################
# agent environment
###############################
AGENT_ENV_CFLAGS = -Wall -Werror -rdynamic -fPIC
AGENT_ENV_CFLAGS += $(ezcd_CPPFLAGS)

#AGENT_ENV_SRC = env_action_base_files.c
#AGENT_ENV_SO = env_action_base_files.so
#AGENT_ENV_EXE = env_action_base_files.exe

#AGENT_ENV_SRC += env_action_bootstrap.c
#AGENT_ENV_SO += env_action_bootstrap.so
#AGENT_ENV_EXE += env_action_bootstrap.exe

if CONFIG_EZBOX_EZCFG_CGROUP
#AGENT_ENV_SRC += env_action_cgroup.c
#AGENT_ENV_SO += env_action_cgroup.so
#AGENT_ENV_EXE += env_action_cgroup.exe
endif

if CONFIG_EZBOX_BR_LAN_NIC
#AGENT_ENV_SRC += env_action_br_lan_if.c
#AGENT_ENV_SO += env_action_br_lan_if.so
#AGENT_ENV_EXE += env_action_br_lan_if.exe
endif

#AGENT_ENV_SRC += env_action_swap_partition.c
#AGENT_ENV_SO += env_action_swap_partition.so
#AGENT_ENV_EXE += env_action_swap_partition.exe

#AGENT_ENV_SRC += env_action_data_storage.c
#AGENT_ENV_SO += env_action_data_storage.so
#AGENT_ENV_EXE += env_action_data_storage.exe

#AGENT_ENV_SRC += env_action_data_partition.c
#AGENT_ENV_SO += env_action_data_partition.so
#AGENT_ENV_EXE += env_action_data_partition.exe

# LUKS/dm-crypt for data partition service
if CONFIG_EZBOX_SERVICE_DMCRYPT_DATA_PARTITION
#AGENT_ENV_SRC += env_action_dmcrypt_data_partition.c
#AGENT_ENV_SO += env_action_dmcrypt_data_partition.so
#AGENT_ENV_EXE += env_action_dmcrypt_data_partition.exe
endif

# network
if CONFIG_EZBOX_ETH_LAN_NIC
#AGENT_ENV_SRC += env_action_eth_lan_if.c
#AGENT_ENV_SO += env_action_eth_lan_if.so
#AGENT_ENV_EXE += env_action_eth_lan_if.exe
endif

#AGENT_ENV_SRC += env_action_ezcd.c
#AGENT_ENV_SO += env_action_ezcd.so
#AGENT_ENV_EXE += env_action_ezcd.exe

#AGENT_ENV_SRC += env_action_ezcm.c
#AGENT_ENV_SO += env_action_ezcm.so
#AGENT_ENV_EXE += env_action_ezcm.exe

if CONFIG_EZBOX_SERVICE_IPTABLES
#AGENT_ENV_SRC += env_action_iptables.c
#AGENT_ENV_SO += env_action_iptables.so
#AGENT_ENV_EXE += env_action_iptables.exe
endif

# klogd service
if CONFIG_EZBOX_SERVICE_KLOGD
#AGENT_ENV_SRC += env_action_klogd.c
#AGENT_ENV_SO += env_action_klogd.so
#AGENT_ENV_EXE += env_action_klogd.exe
endif

if CONFIG_EZBOX_LAN_NIC
#AGENT_ENV_SRC += env_action_lan.c
#AGENT_ENV_SO += env_action_lan.so
#AGENT_ENV_EXE += env_action_lan.exe
endif

#AGENT_ENV_SRC += env_action_load_modules.c
#AGENT_ENV_SO += env_action_load_modules.so
#AGENT_ENV_EXE += env_action_load_modules.exe

#AGENT_ENV_SRC += env_action_login.c
#AGENT_ENV_SO += env_action_login.so
#AGENT_ENV_EXE += env_action_login.exe

#AGENT_ENV_SRC += env_action_loopback.c
#AGENT_ENV_SO += env_action_loopback.so
#AGENT_ENV_EXE += env_action_loopback.exe

#AGENT_ENV_SRC += env_action_mount_system_fs.c
#AGENT_ENV_SO += env_action_mount_system_fs.so
#AGENT_ENV_EXE += env_action_mount_system_fs.exe

#AGENT_ENV_SRC += env_action_netbase.c
#AGENT_ENV_SO += env_action_netbase.so
#AGENT_ENV_EXE += env_action_netbase.exe

#AGENT_ENV_SRC += env_action_nvram.c
#AGENT_ENV_SO += env_action_nvram.so
#AGENT_ENV_EXE += env_action_nvram.exe

if CONFIG_EZBOX_SERVICE_OPENSSL
#AGENT_ENV_SRC += env_action_openssl.c
#AGENT_ENV_SO += env_action_openssl.so
#AGENT_ENV_EXE += env_action_openssl.exe
endif

#AGENT_ENV_SRC += env_action_overlay_rootfs.c
#AGENT_ENV_SO += env_action_overlay_rootfs.so
#AGENT_ENV_EXE += env_action_overlay_rootfs.exe

# PPP service
if CONFIG_EZBOX_SERVICE_PPP
#AGENT_ENV_SRC += env_action_ppp_chap_secrets.c
#AGENT_ENV_SO += env_action_ppp_chap_secrets.so
#AGENT_ENV_EXE += env_action_ppp_chap_secrets.exe

#AGENT_ENV_SRC += env_action_ppp_pap_secrets.c
#AGENT_ENV_SO += env_action_ppp_pap_secrets.so
#AGENT_ENV_EXE += env_action_ppp_pap_secrets.exe
endif

# PPP RP-PPPoE service
if CONFIG_EZBOX_SERVICE_PPP_RP_PPPOE
#AGENT_ENV_SRC += env_action_ppp_rp_pppoe.c
#AGENT_ENV_SO += env_action_ppp_rp_pppoe.so
#AGENT_ENV_EXE += env_action_ppp_rp_pppoe.exe
endif

#AGENT_ENV_SRC += env_action_preboot.c
#AGENT_ENV_SO += env_action_preboot.so
#AGENT_ENV_EXE += env_action_preboot.exe

#AGENT_ENV_SRC += env_action_shelld.c
#AGENT_ENV_SO += env_action_shelld.so
#AGENT_ENV_EXE += env_action_shelld.exe

#AGENT_ENV_SRC += env_action_state.c
#AGENT_ENV_SO += env_action_state.so
#AGENT_ENV_EXE += env_action_state.exe

# syslogd service
if CONFIG_EZBOX_SERVICE_SYSLOGD
#AGENT_ENV_SRC += env_action_syslogd.c
#AGENT_ENV_SO += env_action_syslogd.so
#AGENT_ENV_EXE += env_action_syslogd.exe
endif

#AGENT_ENV_SRC += env_action_system.c
#AGENT_ENV_SO += env_action_system.so
#AGENT_ENV_EXE += env_action_system.exe

#AGENT_ENV_SRC += env_action_udev_pop_nodes.c
#AGENT_ENV_SO += env_action_udev_pop_nodes.so
#AGENT_ENV_EXE += env_action_udev_pop_nodes.exe

if CONFIG_EZBOX_ETH_WAN_NIC
#AGENT_ENV_SRC += env_action_eth_wan_if.c
#AGENT_ENV_SO += env_action_eth_wan_if.so
#AGENT_ENV_EXE += env_action_eth_wan_if.exe
endif

if CONFIG_EZBOX_WAN_NIC
#AGENT_ENV_SRC += env_action_wan.c
#AGENT_ENV_SO += env_action_wan.so
#AGENT_ENV_EXE += env_action_wan.exe

#AGENT_ENV_SRC += env_action_wan_if.c
#AGENT_ENV_SO += env_action_wan_if.so
#AGENT_ENV_EXE += env_action_wan_if.exe
endif

if CONFIG_EZBOX_WIFI_LAN_NIC
#AGENT_ENV_SRC += env_action_wifi_lan_if.c
#AGENT_ENV_SO += env_action_wifi_lan_if.so
#AGENT_ENV_EXE += env_action_wifi_lan_if.exe
endif

#AGENT_ENV_SO = $(subst .c,.so,$(AGENT_ENV_SRC))
#AGENT_ENV_EXE = $(subst .c,.exe,$(AGENT_ENV_SRC))


#%.so : ../src/rcso/%.c
#	$(CC) $(RCSO_CFLAGS) -Wl,-Map,$@.map -L. -lezcd -ldl -shared $< -o $@

#%.exe : ../src/rcso/%.c
#	$(CC) $(RCSO_CFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -D_EXEC_ $< -o $@ -L. -lezcd -lezcfg -ldl

$(RCSO_SO): %.so: ../src/rcso/%.c
	$(CC) $(RCSO_CFLAGS) -Wl,-Map,$@.map -L. -lezcd -ldl -shared $< -o $@

$(RCSO_EXE): %.exe: ../src/rcso/%.c
	$(CC) $(RCSO_CFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -D_EXEC_ $< -o $@ -L. -lezcd -lezcfg -ldl

$(AGENT_ENV_SO): %.so: ../src/rcso/%.c
	$(CC) $(AGENT_ENV_CFLAGS) -Wl,-Map,$@.map -L. -lezcd -ldl -shared $< -o $@

$(AGENT_ENV_EXE): %.exe: ../src/rcso/%.c
	$(CC) $(AGENT_ENV_CFLAGS) $(CFLAGS) $(LDFLAGS) $(LIBS) -D_EXEC_ $< -o $@ -L. -lezcd -lezcfg -ldl

clean-local: clean-local-check
clean-local-check:
	-rm -rf *.so *.exe

.PHONY: clean-local-check

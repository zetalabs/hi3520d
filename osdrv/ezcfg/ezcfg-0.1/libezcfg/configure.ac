#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([libezcfg], [0.1], [ezbox@ezidc.net])
AC_CANONICAL_SYSTEM
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR
LT_INIT

#AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])

AC_LANG(C)

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
#AC_PROG_LIBTOOL

# Configure variables: EZBOX_DISTRO.
AC_ARG_WITH([ezbox-distro],
	AS_HELP_STRING([--with-ezbox-distro=hundun], [which distro ezcfg is built for]),
	[], [with_ezbox_distro=hundun])
if test "x$with_ezbox_distro" = xkuafu; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_KUAFU"
fi
if test "x$with_ezbox_distro" = xhuangdi; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_HUANGDI"
fi
if test "x$with_ezbox_distro" = xjingwei; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_JINGWEI"
fi
if test "x$with_ezbox_distro" = xqiaochui; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_QIAOCHUI"
fi
if test "x$with_ezbox_distro" = xlilou; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_LILOU"
fi
if test "x$with_ezbox_distro" = xfuxi; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_FUXI"
fi
if test "x$with_ezbox_distro" = xzhuanxu; then
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_ZHUANXU"
fi
AC_SUBST([EZBOX_DISTRO_DEFINE])

# Not specified?
if test -z "$EZBOX_DISTRO_DEFINE"; then
	# Set to hundun distro
	EZBOX_DISTRO_DEFINE="-DCONFIG_EZCFG_EZBOX_DISTRO_HUNDUN"
fi

# Configure check unit test support.
config_ezbox_ezcfg_test=no
AC_MSG_CHECKING([if ezcfg_test is enabled])
AC_ARG_ENABLE([ezcfg_test],
	AS_HELP_STRING([--enable-ezcfg_test], [Enable ezcfg_test (default no)]),
	[case "$enableval" in
	yes)
dnl Check if we have check...

dnl PKG_CHECK_MODULES([CHECK],[check >= 0.8.2], config_ezbox_ezcfg_test=yes, config_ezbox_ezcfg_test=no)
CHECK_CHECK([0.8.2], config_ezbox_ezcfg_test=yes, config_ezbox_ezcfg_test=no)
if test "x$config_ezbox_ezcfg_test" = xyes ; then
		AC_DEFINE([HAVE_EZBOX_EZCFG_TEST], [1], [Enable ezcfg_test])
		AC_MSG_RESULT(yes)
		config_ezbox_ezcfg_test=yes
else
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_test=no
fi
	;;
	*)
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_test=no
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_EZCFG_TEST, test "x$config_ezbox_ezcfg_test" = xyes)

# Configure ezcfg sysup support.
config_ezbox_ezcfg_sysup=no
AC_MSG_CHECKING([if ezcfg_sysup is enabled])
AC_ARG_ENABLE([ezcfg_sysup],
	AS_HELP_STRING([--enable-ezcfg_sysup], [Enable ezcfg_sysup (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_EZCFG_SYSUP], [1], [Enable ezcfg_sysup])
		AC_MSG_RESULT(yes)
		config_ezbox_ezcfg_sysup=yes
	;;
	*)
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_sysup=no
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_EZCFG_SYSUP, test "x$config_ezbox_ezcfg_sysup" = xyes)

# Configure ezcfg nls support.
config_ezbox_ezcfg_nls=no
AC_MSG_CHECKING([if ezcfg_nls is enabled])
AC_ARG_ENABLE([ezcfg_nls],
	AS_HELP_STRING([--enable-ezcfg_nls], [Enable ezcfg_nls (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_EZCFG_NLS], [1], [Enable ezcfg_nls])
		AC_MSG_RESULT(yes)
		config_ezbox_ezcfg_nls=yes
	;;
	*)
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_nls=no
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_EZCFG_NLS, test "x$config_ezbox_ezcfg_nls" = xyes)

# Configure ezcfg IPv6 support.
config_ezbox_ezcfg_ipv6=no
AC_MSG_CHECKING([if ezcfg_ipv6 is enabled])
AC_ARG_ENABLE([ezcfg_ipv6],
	AS_HELP_STRING([--enable-ezcfg_ipv6], [Enable ezcfg_ipv6 (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_EZCFG_IPV6], [1], [Enable ezcfg_ipv6])
		AC_MSG_RESULT(yes)
		config_ezbox_ezcfg_ipv6=yes
	;;
	*)
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_ipv6=no
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_EZCFG_IPV6, test "x$config_ezbox_ezcfg_ipv6" = xyes)

# Configure ezcfg cgroup support.
config_ezbox_ezcfg_cgroup=no
AC_MSG_CHECKING([if ezcfg_cgroup is enabled])
AC_ARG_ENABLE([ezcfg_cgroup],
	AS_HELP_STRING([--enable-ezcfg_cgroup], [Enable ezcfg_cgroup (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_EZCFG_CGROUP], [1], [Enable ezcfg_cgroup])
		AC_MSG_RESULT(yes)
		config_ezbox_ezcfg_cgroup=yes
	;;
	*)
		AC_MSG_RESULT(no)
		config_ezbox_ezcfg_cgroup=no
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_EZCFG_CGROUP, test "x$config_ezbox_ezcfg_cgroup" = xyes)

# Configure EtherLAN NIC.
config_ezbox_eth_lan_nic=yes
AC_MSG_CHECKING([if Ethernet LAN network interface is enabled])
AC_ARG_ENABLE([eth-lan-nic],
	AS_HELP_STRING([--enable-eth-lan-nic], [Enable Ethernet LAN network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_eth_lan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_ETH_LAN_NIC], [1], [Enable Ethernet LAN network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_ETH_LAN_NIC], [1], [Enable Ethernet LAN network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_ETH_LAN_NIC, test "x$config_ezbox_eth_lan_nic" = xyes)

# Configure WifiLAN NIC.
config_ezbox_wifi_lan_nic=yes
AC_MSG_CHECKING([if Wifi LAN network interface is enabled])
AC_ARG_ENABLE([wifi-lan-nic],
	AS_HELP_STRING([--enable-wifi-lan-nic], [Enable Wifi LAN network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_wifi_lan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_WIFI_LAN_NIC], [1], [Enable Wifi LAN network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_WIFI_LAN_NIC], [1], [Enable Wifi LAN network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_WIFI_LAN_NIC, test "x$config_ezbox_wifi_lan_nic" = xyes)

# Configure LAN bridge NIC.
config_ezbox_br_lan_nic=yes
AC_MSG_CHECKING([if LAN bridge network interface is enabled])
AC_ARG_ENABLE([br-lan-nic],
	AS_HELP_STRING([--enable-br-lan-nic], [Enable LAN bridge network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_br_lan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_BR_LAN_NIC], [1], [Enable LAN bridge network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_BR_LAN_NIC], [1], [Enable LAN bridge network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_BR_LAN_NIC, test "x$config_ezbox_br_lan_nic" = xyes)

# Configure LAN NIC.
config_ezbox_lan_nic=yes
AC_MSG_CHECKING([if LAN network interface is enabled])
AC_ARG_ENABLE([lan-nic],
	AS_HELP_STRING([--enable-lan-nic], [Enable LAN network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_lan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_LAN_NIC], [1], [Enable LAN network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_LAN_NIC], [1], [Enable LAN network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_LAN_NIC, test "x$config_ezbox_lan_nic" = xyes)

# Configure EtherWAN NIC.
config_ezbox_eth_wan_nic=yes
AC_MSG_CHECKING([if Ethernet WAN network interface is enabled])
AC_ARG_ENABLE([eth-wan-nic],
	AS_HELP_STRING([--enable-eth-wan-nic], [Enable Ethernet WAN network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_eth_wan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_ETH_WAN_NIC], [1], [Enable Ethernet WAN network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_ETH_WAN_NIC], [1], [Enable Ethernet WAN network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_ETH_WAN_NIC, test "x$config_ezbox_eth_wan_nic" = xyes)

# Configure WAN NIC.
config_ezbox_wan_nic=yes
AC_MSG_CHECKING([if WAN network interface is enabled])
AC_ARG_ENABLE([wan-nic],
	AS_HELP_STRING([--enable-wan-nic], [Enable WAN network interface (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_wan_nic=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_WAN_NIC], [1], [Enable WAN network interface])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	[AC_DEFINE([HAVE_EZBOX_WAN_NIC], [1], [Enable WAN network interface]) AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_WAN_NIC, test "x$config_ezbox_wan_nic" = xyes)

# Configure WAN connection type PPPoE.
config_ezbox_wan_pppoe=no
AC_MSG_CHECKING([if WAN type PPPoE is enabled])
AC_ARG_ENABLE([wan-pppoe],
	AS_HELP_STRING([--enable-wan-pppoe], [Enable WAN connection type PPPoE (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_WAN_PPPOE], [1], [Enable WAN connection type PPPoE])
		AC_MSG_RESULT(yes)
		config_ezbox_wan_pppoe=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_WAN_PPPOE, test "x$config_ezbox_wan_pppoe" = xyes)

# Configure WAN connection type PPTP.
config_ezbox_wan_pptp=no
AC_MSG_CHECKING([if WAN type PPTP is enabled])
AC_ARG_ENABLE([wan-pptp],
	AS_HELP_STRING([--enable-wan-pptp], [Enable WAN connection type PPTP (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_WAN_PPTP], [1], [Enable WAN connection type PPTP])
		AC_MSG_RESULT(yes)
		config_ezbox_wan_pptp=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_WAN_PPTP, test "x$config_ezbox_wan_pptp" = xyes)

# Configure WAN connection type L2TP.
config_ezbox_wan_l2tp=no
AC_MSG_CHECKING([if WAN type L2TP is enabled])
AC_ARG_ENABLE([wan-l2tp],
	AS_HELP_STRING([--enable-wan-l2tp], [Enable WAN connection type L2TP (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_WAN_L2TP], [1], [Enable WAN connection type L2TP])
		AC_MSG_RESULT(yes)
		config_ezbox_wan_l2tp=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_WAN_L2TP, test "x$config_ezbox_wan_l2tp" = xyes)

# Configure openssl service.
config_ezbox_service_openssl=no
AC_MSG_CHECKING([if openssl service is enabled])
AC_ARG_ENABLE([service-openssl],
	AS_HELP_STRING([--enable-service-openssl], [Enable openssl service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_OPENSSL], [1], [Enable openssl service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_openssl=yes
		LIBS="-lssl -lcrypto $LIBS"
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_OPENSSL, test "x$config_ezbox_service_openssl" = xyes)

# Configure syslog service.
config_ezbox_service_syslogd=no
AC_MSG_CHECKING([if syslog service is enabled])
AC_ARG_ENABLE([service-syslogd],
	AS_HELP_STRING([--enable-service-syslogd], [Enable syslog service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_SYSLOGD], [1], [Enable syslog service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_syslogd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_SYSLOGD, test "x$config_ezbox_service_syslogd" = xyes)

# Configure klog service.
config_ezbox_service_klogd=no
AC_MSG_CHECKING([if klog service is enabled])
AC_ARG_ENABLE([service-klogd],
	AS_HELP_STRING([--enable-service-klogd], [Enable klog service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_KLOGD], [1], [Enable klog service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_klogd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_KLOGD, test "x$config_ezbox_service_klogd" = xyes)

# Configure ezcfg_httpd service.
config_ezbox_service_ezcfg_httpd=yes
AC_MSG_CHECKING([if ezcfg_httpd service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd],
	AS_HELP_STRING([--enable-service-ezcfg_httpd], [Enable ezcfg_httpd service (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_service_ezcfg_httpd=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD], [1], [Enable ezcfg_httpd service])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD], [1], [Enable ezcfg_httpd service])
	[AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD, test "x$config_ezbox_service_ezcfg_httpd" = xyes)

# Configure ezcfg_httpd_cgi_index service.
config_ezbox_service_ezcfg_httpd_cgi_index=no
AC_MSG_CHECKING([if ezcfg_httpd_cgi_index service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_cgi_index],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_cgi_index], [Enable ezcfg_httpd_cgi_index service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_INDEX], [1], [Enable ezcfg_httpd_cgi_index service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_cgi_index=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_INDEX, test "x$config_ezbox_service_ezcfg_httpd_cgi_index" = xyes)

# Configure ezcfg_httpd_cgi_admin service.
config_ezbox_service_ezcfg_httpd_cgi_admin=no
AC_MSG_CHECKING([if ezcfg_httpd_cgi_admin service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_cgi_admin],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_cgi_admin], [Enable ezcfg_httpd_cgi_admin service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN], [1], [Enable ezcfg_httpd_cgi_admin service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_cgi_admin=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_ADMIN, test "x$config_ezbox_service_ezcfg_httpd_cgi_admin" = xyes)

# Configure ezcfg_httpd_cgi_nvram service.
config_ezbox_service_ezcfg_httpd_cgi_nvram=no
AC_MSG_CHECKING([if ezcfg_httpd_cgi_nvram service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_cgi_nvram],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_cgi_nvram], [Enable ezcfg_httpd_cgi_nvram service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM], [1], [Enable ezcfg_httpd_cgi_nvram service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_cgi_nvram=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_NVRAM, test "x$config_ezbox_service_ezcfg_httpd_cgi_nvram" = xyes)

# Configure ezcfg_httpd_cgi_apply service.
config_ezbox_service_ezcfg_httpd_cgi_apply=no
AC_MSG_CHECKING([if ezcfg_httpd_cgi_apply service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_cgi_apply],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_cgi_apply], [Enable ezcfg_httpd_cgi_apply service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY], [1], [Enable ezcfg_httpd_cgi_apply service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_cgi_apply=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_CGI_APPLY, test "x$config_ezbox_service_ezcfg_httpd_cgi_apply" = xyes)

# Configure ezcfg_httpd_ssi service.
config_ezbox_service_ezcfg_httpd_ssi=no
AC_MSG_CHECKING([if ezcfg_httpd_ssi service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_ssi],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_ssi], [Enable ezcfg_httpd_ssi service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_SSI], [1], [Enable ezcfg_httpd_ssi service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_ssi=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_SSI, test "x$config_ezbox_service_ezcfg_httpd_ssi" = xyes)

# Configure ezcfg_httpd_websocket service.
config_ezbox_service_ezcfg_httpd_websocket=no
AC_MSG_CHECKING([if ezcfg_httpd_websocket service is enabled])
AC_ARG_ENABLE([service-ezcfg_httpd_websocket],
	AS_HELP_STRING([--enable-service-ezcfg_httpd_websocket], [Enable ezcfg_httpd_websocket service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_httpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET], [1], [Enable ezcfg_httpd_websocket service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_httpd_websocket=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_HTTPD_WEBSOCKET, test "x$config_ezbox_service_ezcfg_httpd_websocket" = xyes)

# Configure ezcfg_igrsd service.
config_ezbox_service_ezcfg_igrsd=yes
AC_MSG_CHECKING([if ezcfg_igrsd service is enabled])
AC_ARG_ENABLE([service-ezcfg_igrsd],
	AS_HELP_STRING([--enable-service-ezcfg_igrsd], [Enable ezcfg_igrsd service (default yes)]),
	[case "$enableval" in
	no)
		AC_MSG_RESULT(no)
		config_ezbox_service_ezcfg_igrsd=no
	;;
	*)
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_IGRSD], [1], [Enable ezcfg_igrsd service])
		AC_MSG_RESULT(yes)
	;;
	esac] ,
	AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_IGRSD], [1], [Enable ezcfg_igrsd service])
	[AC_MSG_RESULT(yes)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_IGRSD, test "x$config_ezbox_service_ezcfg_igrsd" = xyes)

# Configure ezcfg_upnpd service.
config_ezbox_service_ezcfg_upnpd=no
AC_MSG_CHECKING([if ezcfg_upnpd service is enabled])
AC_ARG_ENABLE([service-ezcfg_upnpd],
	AS_HELP_STRING([--enable-service-ezcfg_upnpd], [Enable ezcfg_upnpd service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_UPNPD], [1], [Enable ezcfg_upnpd service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_upnpd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_UPNPD, test "x$config_ezbox_service_ezcfg_upnpd" = xyes)

# Configure ezcfg_upnpd_igd1 service.
config_ezbox_service_ezcfg_upnpd_igd1=no
AC_MSG_CHECKING([if UPnP IGD1 service is enabled])
AC_ARG_ENABLE([service-ezcfg_upnpd_igd1],
	AS_HELP_STRING([--enable-service-ezcfg_upnpd_igd1], [Enable UPnP IGD1 service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_ezcfg_upnpd" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCFG_UPNPD_IGD1], [1], [Enable UPnP IGD1 service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezcfg_upnpd_igd1=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCFG_UPNPD_IGD1, test "x$config_ezbox_service_ezcfg_upnpd_igd1" = xyes)

# Configure OpenSSL service.
config_ezbox_service_openssl=no
AC_MSG_CHECKING([if openssl service is enabled])
AC_ARG_ENABLE([service-openssl],
	AS_HELP_STRING([--enable-service-openssl], [Enable OpenSSL service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_OPENSSL], [1], [Enable OpenSSL service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_openssl=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_OPENSSL, test "x$config_ezbox_service_openssl" = xyes)

# Configure telnet service.
config_ezbox_service_telnetd=no
AC_MSG_CHECKING([if telnetd service is enabled])
AC_ARG_ENABLE([service-telnetd],
	AS_HELP_STRING([--enable-service-telnetd], [Enable telnet service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_TELNETD], [1], [Enable telnet service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_telnetd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_TELNETD, test "x$config_ezbox_service_telnetd" = xyes)

# Configure dnsmasq service.
config_ezbox_service_dnsmasq=no
AC_MSG_CHECKING([if dnsmasq service is enabled])
AC_ARG_ENABLE([service-dnsmasq],
	AS_HELP_STRING([--enable-service-dnsmasq], [Enable dnsmasq service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_DNSMASQ], [1], [Enable dnsmasq service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_dnsmasq=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_DNSMASQ, test "x$config_ezbox_service_dnsmasq" = xyes)

# Configure iptables service.
config_ezbox_service_iptables=no
AC_MSG_CHECKING([if iptables service is enabled])
AC_ARG_ENABLE([service-iptables],
	AS_HELP_STRING([--enable-service-iptables], [Enable iptables service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_IPTABLES], [1], [Enable iptables service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_iptables=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_IPTABLES, test "x$config_ezbox_service_iptables" = xyes)

# Configure iptables firewall service.
config_ezbox_service_iptables_firewall=no
AC_MSG_CHECKING([if iptables firewall service is enabled])
AC_ARG_ENABLE([service-iptables_firewall],
	AS_HELP_STRING([--enable-service-iptables_firewall], [Enable iptables firewall service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_iptables" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_IPTABLES_FIREWALL], [1], [Enable iptables firewall service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_iptables_firewall=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_IPTABLES_FIREWALL, test "x$config_ezbox_service_iptables_firewall" = xyes)

# Configure iptables NAT service.
config_ezbox_service_iptables_nat=no
AC_MSG_CHECKING([if iptables NAT service is enabled])
AC_ARG_ENABLE([service-iptables_nat],
	AS_HELP_STRING([--enable-service-iptables_nat], [Enable iptables NAT service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_iptables" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_IPTABLES_NAT], [1], [Enable iptables NAT service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_iptables_nat=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_IPTABLES_NAT, test "x$config_ezbox_service_iptables_nat" = xyes)

# Configure iptables DMZ service.
config_ezbox_service_iptables_dmz=no
AC_MSG_CHECKING([if iptables DMZ service is enabled])
AC_ARG_ENABLE([service-iptables_dmz],
	AS_HELP_STRING([--enable-service-iptables_dmz], [Enable iptables DMZ service (default no)]),
	[case "$enableval" in
	yes)
if test "x$config_ezbox_service_iptables" = xyes; then
		AC_DEFINE([HAVE_EZBOX_SERVICE_IPTABLES_DMZ], [1], [Enable iptables DMZ service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_iptables_dmz=yes
else
		AC_MSG_RESULT(no)
fi
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_IPTABLES_DMZ, test "x$config_ezbox_service_iptables_dmz" = xyes)

# Configure dhid service.
config_ezbox_service_dhid=no
AC_MSG_CHECKING([if DHIS client service is enabled])
AC_ARG_ENABLE([service-dhid],
	AS_HELP_STRING([--enable-service-dhid], [Enable DHIS client service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_DHID], [1], [Enable DHIS client service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_dhid=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_DHID, test "x$config_ezbox_service_dhid" = xyes)

# Configure dhisd service.
config_ezbox_service_dhisd=no
AC_MSG_CHECKING([if DHIS server service is enabled])
AC_ARG_ENABLE([service-dhisd],
	AS_HELP_STRING([--enable-service-dhisd], [Enable DHIS server service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_DHISD], [1], [Enable DHIS server service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_dhisd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_DHISD, test "x$config_ezbox_service_dhisd" = xyes)

# Configure linux unified key setup (LUKS) for data partition service.
config_ezbox_service_dmcrypt_data_partition=no
AC_MSG_CHECKING([if LUKS/dm-crypt for data partition service is enabled])
AC_ARG_ENABLE([service-dmcrypt_data_partition],
	AS_HELP_STRING([--enable-service-dmcrypt_data_partition], [Enable LUKS/dm-crypt for data partition service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_DMCRYPT_DATA_PARTITION], [1], [Enable LUKS/dm-crypt for data partition service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_dmcrypt_data_partition=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_DMCRYPT_DATA_PARTITION, test "x$config_ezbox_service_dmcrypt_data_partition" = xyes)

# Configure ezctp service.
config_ezbox_service_ezctp=no
AC_MSG_CHECKING([if ezctp service is enabled])
AC_ARG_ENABLE([service-ezctp],
	AS_HELP_STRING([--enable-service-ezctp], [Enable ezctp service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_EZCTP], [1], [Enable ezctp service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ezctp=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EZCTP, test "x$config_ezbox_service_ezctp" = xyes)

# Configure nano-X service.
config_ezbox_service_nano_x=no
AC_MSG_CHECKING([if nano-X service is enabled])
AC_ARG_ENABLE([service-nano_x],
	AS_HELP_STRING([--enable-service-nano_x], [Enable nano-X service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_NANO_X], [1], [Enable nano-X service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_nano_x=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_NANO_X, test "x$config_ezbox_service_nano_x" = xyes)

# Configure TinyX/kdrive service.
config_ezbox_service_kdrive=no
AC_MSG_CHECKING([if TinyX/kdrive service is enabled])
AC_ARG_ENABLE([service-kdrive],
	AS_HELP_STRING([--enable-service-kdrive], [Enable TinyX/kdrive service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_KDRIVE], [1], [Enable TinyX/kdrive service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_kdrive=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_KDRIVE, test "x$config_ezbox_service_kdrive" = xyes)

# Configure fontconfig service.
config_ezbox_service_fontconfig=no
AC_MSG_CHECKING([if fontconfig service is enabled])
AC_ARG_ENABLE([service-fontconfig],
	AS_HELP_STRING([--enable-service-fontconfig], [Enable fontconfig service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_FONTCONFIG], [1], [Enable fontconfig service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_fontconfig=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_FONTCONFIG, test "x$config_ezbox_service_fontconfig" = xyes)

# Configure dillo web browser service.
config_ezbox_service_dillo=no
AC_MSG_CHECKING([if dillo web browser service is enabled])
AC_ARG_ENABLE([service-dillo],
	AS_HELP_STRING([--enable-service-dillo], [Enable dillo web browser service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_DILLO], [1], [Enable dillo web browser service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_dillo=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_DILLO, test "x$config_ezbox_service_dillo" = xyes)

# Configure wpa_supplicant service.
config_ezbox_service_wpa_supplicant=no
AC_MSG_CHECKING([if wpa_supplicant service is enabled])
AC_ARG_ENABLE([service-wpa_supplicant],
	AS_HELP_STRING([--enable-service-wpa_supplicant], [Enable wpa_supplicant service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_WPA_SUPPLICANT], [1], [Enable wpa_supplicant service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_wpa_supplicant=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_WPA_SUPPLICANT, test "x$config_ezbox_service_wpa_supplicant" = xyes)

# Configure EMC2 (Enhanced Machine Controller) service.
config_ezbox_service_emc2=no
AC_MSG_CHECKING([if EMC2 Enhanced Machine Controller service is enabled])
AC_ARG_ENABLE([service-emc2],
	AS_HELP_STRING([--enable-service-emc2], [Enable EMC2 Enhanced Machine Controller service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_EMC2], [1], [Enable EMC2 Enhanced Machine Controller service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_emc2=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_EMC2, test "x$config_ezbox_service_emc2" = xyes)

# Configure coova-chilli Hotspot Manager service.
config_ezbox_service_coova_chilli=no
AC_MSG_CHECKING([if coova-chilli Hotspot Manager service is enabled])
AC_ARG_ENABLE([service-coova_chilli],
	AS_HELP_STRING([--enable-service-coova_chilli], [Enable coova-chilli Hotspot Manager service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_COOVA_CHILLI], [1], [Enable coova-chilli Hotspot Manager service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_coova_chilli=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_COOVA_CHILLI, test "x$config_ezbox_service_coova_chilli" = xyes)

# Configure LXC (Linux Container) service.
config_ezbox_service_lxc=no
AC_MSG_CHECKING([if LXC Linux Container service is enabled])
AC_ARG_ENABLE([service-lxc],
	AS_HELP_STRING([--enable-service-lxc], [Enable LXC Linux Container service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_LXC], [1], [Enable LXC Linux Container service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_lxc=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_LXC, test "x$config_ezbox_service_lxc" = xyes)

# Configure radvd (Linux IPv6 Router Advertisement Daemon) service.
config_ezbox_service_radvd=no
AC_MSG_CHECKING([if radvd Linux IPv6 Router Advertisement Daemon service is enabled])
AC_ARG_ENABLE([service-radvd],
	AS_HELP_STRING([--enable-service-radvd], [Enable radvd Linux IPv6 Router Advertisement Daemon service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_RADVD], [1], [Enable radvd Linux IPv6 Router Advertisement Daemon service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_radvd=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_RADVD, test "x$config_ezbox_service_radvd" = xyes)

# Configure ppp service.
config_ezbox_service_ppp=no
AC_MSG_CHECKING([if PPP service is enabled])
AC_ARG_ENABLE([service-ppp],
	AS_HELP_STRING([--enable-service-ppp], [Enable PPP service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_PPP], [1], [Enable PPP service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ppp=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_PPP, test "x$config_ezbox_service_ppp" = xyes)

# Configure pppoe-server service.
config_ezbox_service_pppoe_server=no
AC_MSG_CHECKING([if PPPoE server service is enabled])
AC_ARG_ENABLE([service-pppoe_server],
	AS_HELP_STRING([--enable-service-pppoe_server], [Enable PPPoE server service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_PPPOE_SERVER], [1], [Enable PPPoE server service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_pppoe_server=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_PPPOE_SERVER, test "x$config_ezbox_service_pppoe_server" = xyes)

# Configure ppp rp-pppoe service.
config_ezbox_service_ppp_rp_pppoe=no
AC_MSG_CHECKING([if PPP RP-PPPoE service is enabled])
AC_ARG_ENABLE([service-ppp_rp_pppoe],
	AS_HELP_STRING([--enable-service-ppp_rp_pppoe], [Enable PPP RP-PPPoE service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_PPP_RP_PPPOE], [1], [Enable PPP RP-PPPoE service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_ppp_rp_pppoe=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_PPP_RP_PPPOE, test "x$config_ezbox_service_ppp_rp_pppoe" = xyes)

# Configure freeradius2 server service.
config_ezbox_service_freeradius2_server=no
AC_MSG_CHECKING([if FreeRADIUS2 server service is enabled])
AC_ARG_ENABLE([service-freeradius2_server],
	AS_HELP_STRING([--enable-service-freeradius2_server], [Enable FreeRADIUS2 server service (default no)]),
	[case "$enableval" in
	yes)
		AC_DEFINE([HAVE_EZBOX_SERVICE_FREERADIUS2_SERVER], [1], [Enable FreeRADIUS2 server service])
		AC_MSG_RESULT(yes)
		config_ezbox_service_freeradius2_server=yes
	;;
	*)
		AC_MSG_RESULT(no)
	;;
	esac] ,
	[AC_MSG_RESULT(no)]
)
AM_CONDITIONAL(CONFIG_EZBOX_SERVICE_FREERADIUS2_SERVER, test "x$config_ezbox_service_freeradius2_server" = xyes)

# Checks for libraries.
# This macro is defined in check.m4 and tests if check.h and
# libcheck.a are installed in your system. It sets CHECK_CFLAGS and
# CHECK_LIBS accordingly.  
#  AM_PATH_CHECK([MINIMUM-VERSION,
#                [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
#AM_PATH_CHECK(,[have_check="yes"],
#	AC_MSG_WARN([Check not found; cannot run unit tests!])
#	[have_check="no"])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h netinet/in.h stddef.h stdlib.h string.h sys/mount.h sys/socket.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT64_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_HEADER_MAJOR
AC_FUNC_MALLOC
AC_CHECK_FUNCS([dup2 memset mkdir select socket strchr strerror strrchr])

# Checks for POSIX Threads
echo "--------------------------- pthread stuff -------------------------------------"
ACX_PTHREAD(
	[],
	[AC_MSG_ERROR([POSIX threads are required to build this program])])
# Update environment variables for pthreads
CC="$PTHREAD_CC"
CFLAGS="$PTHREAD_CFLAGS $CFLAGS"
LIBS="$PTHREAD_LIBS $LIBS"
#
# Determine if pthread_rwlock_t is available
#
echo "----------------------- pthread_rwlock_t stuff --------------------------------"
AC_MSG_CHECKING([if pthread_rwlock_t is available])
AC_LANG([C])
AC_COMPILE_IFELSE(
	[AC_LANG_PROGRAM(
		[#include <pthread.h>],
		[pthread_rwlock_t *x;])],
	[AC_DEFINE([LIBEZCFG_USE_RWLOCK], [1], [Use pthread_rwlock_t])
		AC_MSG_RESULT([yes, supported without any options])],
	[AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM(
			[#define _GNU_SOURCE
			#include <pthread.h>],
			[pthread_rwlock_t *x;])],
		[AC_DEFINE([LIBEZCFG_USE_RWLOCK], [1], [Use pthread_rwlock_t])
			CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE"
			AC_MSG_RESULT([yes, definition of _GNU_SOURCE required])],
		[AC_DEFINE([LIBEZCFG_USE_RWLOCK], [0], [Do not use pthread_rwlock_t])
			AC_MSG_RESULT([no, needs to fallback to pthread_mutex])
			AC_MSG_ERROR([pthread_rwlock_t not available])])])
echo "-------------------------------------------------------------------------------"

AC_MSG_CHECKING([if locale is available])
# Checks for locale header files.
AC_CHECK_HEADERS([locale.h])
# Checks for setlocale function.
AC_CHECK_FUNCS([setlocale])
AC_SUBST(HAVE_LOCALE_H)

AC_MSG_CHECKING([if i18n languages is available])
# Checks for i18n languages header files.
AC_CHECK_HEADERS([libintl.h])
# Checks for gettext function.
AC_CHECK_LIB(intl, gettext)
AC_SUBST(HAVE_LIBINTL_H)
AC_SUBST(HAVE_LIBINTL)


AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
	Makefile
	libezcfg.pc
	lib/Makefile
	tests/Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
	libezcfg $VERSION
	========

	prefix:			${prefix}
	sysconfdir:		${sysconfdir}
	sbindir:		${sbinfir}
	libdir:			${libdir}
	rootlibdir:		${rootlib_execdir}
	libexecdir:		${libexecdir}

	datarootdir:		${datarootdir}
	mandir:			${mandir}
	includedir:		${includedir}

	include_prefix:		${INCLUDE_PREFIX}

	logging:		${enable_logging}
	debug:			${enable_debug}

	compiler:		${CC}
	cflags:			${CFLAGS}
	ldflags:		${LDFLAGS}
	libs:			${LIBS}
	distro define:		${EZBOX_DISTRO_DEFINE}
])

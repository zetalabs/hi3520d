#!/bin/sh -e

autoreconf --verbose --install --symlink --force

EZCFG_CFLAGS="-g -Wall \
-Wmissing-declarations -Wmissing-prototypes \
-Wnested-externs -Wpointer-arith \
-Wpointer-arith -Wsign-compare -Wchar-subscripts \
-Wstrict-prototypes -Wshadow \
-Wformat=2 -Wtype-limits"

case "$CFLAGS" in
	*-O[0-9]*)
		;;
	*)
		EZCFG_CFLAGS="$EZCFG_CFLAGS -O2"
		;;
esac

libdir() {
	echo $(cd $1/$(gcc -print-multi-os-directory); pwd)
}

#args="--prefix=/usr/local \
#--sysconfdir=/etc \
#--sbindir=/sbin \
#--libdir=$(libdir /lib) \
#--libexecdir=/lib/ezcfg \
#--includedir=/usr/include \
#--enable-service-ezcfg_httpd_cgi_index \
#--enable-service-ezcfg_httpd_cgi_admin \
#--enable-service-ezcfg_httpd_cgi_nvram \
#--enable-ezcfg_test \
#--enable-ezcfg_nls"

args="--prefix=/usr/local \
--sysconfdir=/etc \
--with-ezbox-distro=fuxi \
--enable-ezcfg_test \
--enable-ezcfg_ipv6 \
--enable-ezcfg_cgroup \
--enable-wan-pppoe \
--enable-service-ezcfg_httpd \
--enable-service-ezcfg_httpd_cgi_apply \
--enable-service-ezcfg_httpd_ssi \
--enable-service-ezcfg_httpd_websocket \
--enable-service-ezcfg_upnpd \
--enable-service-ezcfg_upnpd_igd1 \
--enable-service-openssl \
--enable-service-iptables \
--enable-service-dhisd \
--enable-service-dhid \
--enable-service-dmcrypt_data_partition \
--enable-service-ezctp \
--enable-service-lxc \
--enable-service-coova_chilli \
--enable-service-radvd \
--enable-service-ppp \
--enable-service-pppoe_server \
--enable-service-ppp_rp_pppoe \
--enable-service-freeradius2_server \
"

export CFLAGS="$CFLAGS $EZCFG_CFLAGS"
./configure $args $@

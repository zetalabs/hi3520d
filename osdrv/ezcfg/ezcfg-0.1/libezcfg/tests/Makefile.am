## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = \
	-include $(top_builddir)/config.h \
	-I$(top_builddir)/include \
	-I$(top_builddir)/api/include \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DEZCFG_DEBUG \
	-Wall -Werror

AM_LDFLAGS = -L. -lezcfg

libezcfg.so: $(top_builddir)/lib/.libs/libezcfg.so
	-rm -f libezcfg.so
	$(LN_S) $(top_builddir)/lib/.libs/libezcfg.so libezcfg.so

TESTS = check_xml check_json check_upnp check_nvram check_ssi
check_PROGRAMS = check_xml check_json check_upnp check_nvram check_ssi

check_xml_DEPENDENCIES = libezcfg.so
check_xml_SOURCES = xml/check_xml.c
check_xml_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_xml_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_xml_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

check_json_DEPENDENCIES = libezcfg.so
check_json_SOURCES = json/check_json.c
check_json_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_json_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_json_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

check_upnp_DEPENDENCIES = libezcfg.so
check_upnp_SOURCES = upnp/check_upnp.c
check_upnp_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_upnp_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_upnp_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

check_nvram_DEPENDENCIES = libezcfg.so
check_nvram_SOURCES = nvram/check_nvram.c
check_nvram_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_nvram_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_nvram_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

check_ssi_DEPENDENCIES = libezcfg.so
check_ssi_SOURCES = ssi/check_ssi.c
check_ssi_CFLAGS = $(AM_CPPFLAGS) $(EZBOX_DISTRO_DEFINE) @CHECK_CFLAGS@
#check_ssi_LDADD = @CHECK_LIBS@ $(top_builddir)/lib/libezcfg.la
check_ssi_LDFLAGS = $(AM_LDFLAGS) @CHECK_LIBS@

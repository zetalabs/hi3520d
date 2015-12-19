#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.62])
AC_INIT([ezcfg], [0.1], [ezbox@ezidc.net])
AC_CANONICAL_SYSTEM
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AM_PROG_AR

#AC_DISABLE_STATIC
AC_CONFIG_MACRO_DIR([m4])
AC_PREFIX_DEFAULT([/usr])

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

AC_CONFIG_SUBDIRS([libezcfg ezcd])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([
	Makefile
])
AC_OUTPUT
AC_MSG_RESULT([
	ezcfg $VERSION
	========

	distro define:	${EZBOX_DISTRO_DEFINE}
])

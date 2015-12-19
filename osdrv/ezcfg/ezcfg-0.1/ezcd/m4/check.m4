
dnl Usage:
dnl CHECK_CHECK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl Adds the required libraries to $CHECK_LIBS and $CHECK_CFLAGS does an
dnl AC_SUBST(CHECK_LIBS) and AC_SUBST(CHECK_CFLAGS)
dnl


AC_DEFUN([CHECK_CHECK],
[
AC_REQUIRE([AC_CANONICAL_HOST])


AC_ARG_WITH(check,
	AS_HELP_STRING([--with-check=PATH],
	               [prefix where check is installed [default=auto]]))

if test x$with_check = xno; then

$3

else



dnl save LIBS
saved_LIBS="$LIBS"
saved_CFGLAGS="$CFLAGS"

if test x$with_check != x; then
	CFLAGS="$CFLAGS -I$with_check/include"
	LIBS="$LIBS -L$with_check/lib -lcheck"
fi

AC_MSG_CHECKING(for check >= $1)

dnl Get major, minor, and micro version from arg MINIMUM-VERSION
check_config_major_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
check_config_minor_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
check_config_micro_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl Compare MINIMUM-VERSION with libezcfg version
AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

int main ()
{
	int		major = CHECK_MAJOR_VERSION,
			minor = CHECK_MINOR_VERSION,
			micro = CHECK_MICRO_VERSION;
	const char	*version;


	if ((major > $check_config_major_version) ||
	   ((major == $check_config_major_version) && (minor > $check_config_minor_version)) ||
	   ((major == $check_config_major_version) && (minor == $check_config_minor_version) && (micro >= $check_config_micro_version))) {

		return 0;
	} else {
		printf(" *** Looked for check version >= %d.%d.%d, found %d.%d.%d\n",
			$check_config_major_version, $check_config_minor_version,
			$check_config_micro_version, major, minor, micro);
		return 1;
	}
}
],
    AC_MSG_RESULT(yes),
    AC_MSG_RESULT(no) ; $3,
    [echo $ac_n "cross compiling; assumed OK... $ac_c"])

dnl restore orignial LIBS and set @CHECK_LIBS@

if test x$with_check != x; then
	CHECK_CFLAGS="-I$with_check/include"
	CHECK_LIBS="-L$with_check/lib -lcheck"
else
	CHECK_LIBS="-lcheck"
	CHECK_CFLAGS=""
fi
AC_SUBST(CHECK_LIBS)
AC_SUBST(CHECK_CFLAGS)
LIBS="$saved_LIBS"


dnl Execute ACTION-IF-FOUND
$2

fi

])


dnl Usage:
dnl CHECK_CHECK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl
dnl Adds the required libraries to $CHECK_LIBS and $CHECK_CFLAGS does an
dnl AC_SUBST(CHECK_LIBS) and AC_SUBST(CHECK_CFLAGS)
dnl


AC_DEFUN([CHECK_CHECK],
[
AC_REQUIRE([AC_CANONICAL_HOST])


AC_ARG_WITH(check,
	AS_HELP_STRING([--with-check=PATH],
	               [prefix where check is installed [default=auto]]))

if test x$with_check = xno; then

$3

else



dnl save LIBS
saved_LIBS="$LIBS"
saved_CFGLAGS="$CFLAGS"

if test x$with_check != x; then
	CFLAGS="$CFLAGS -I$with_check/include"
	LIBS="$LIBS -L$with_check/lib -lcheck"
fi

AC_MSG_CHECKING(for check >= $1)

dnl Get major, minor, and micro version from arg MINIMUM-VERSION
check_config_major_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
check_config_minor_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
check_config_micro_version=`echo $1 | \
    sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

dnl Compare MINIMUM-VERSION with libezcfg version
AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <check.h>

int main ()
{
	int		major = CHECK_MAJOR_VERSION,
			minor = CHECK_MINOR_VERSION,
			micro = CHECK_MICRO_VERSION;
	const char	*version;


	if ((major > $check_config_major_version) ||
	   ((major == $check_config_major_version) && (minor > $check_config_minor_version)) ||
	   ((major == $check_config_major_version) && (minor == $check_config_minor_version) && (micro >= $check_config_micro_version))) {

		return 0;
	} else {
		printf(" *** Looked for check version >= %d.%d.%d, found %d.%d.%d\n",
			$check_config_major_version, $check_config_minor_version,
			$check_config_micro_version, major, minor, micro);
		return 1;
	}
}
],
    AC_MSG_RESULT(yes) ; $2,
    AC_MSG_RESULT(no) ; $3,
    [echo $ac_n "cross compiling; assumed OK... $ac_c"])

dnl restore orignial LIBS and set @CHECK_LIBS@

if test x$with_check != x; then
	CHECK_CFLAGS="-I$with_check/include"
	CHECK_LIBS="-L$with_check/lib -lcheck"
else
	CHECK_LIBS="-lcheck"
	CHECK_CFLAGS=""
fi
AC_SUBST(CHECK_LIBS)
AC_SUBST(CHECK_CFLAGS)
LIBS="$saved_LIBS"

fi

])



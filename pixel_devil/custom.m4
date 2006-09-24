
AC_PREREQ(2.50)

AC_DEFUN(CUSTOM_ENABLE_STATIC_DEVIL, [
	AC_MSG_CHECKING([whether to statically link libIL and libILU])
	AC_ARG_ENABLE(static-devil,
		AC_HELP_STRING([--enable-static-devil],
			[link libIL and libILU statically]),
		[tcl_ok=$enableval], [tcl_ok=no])

	if test "${enable_static_devil+set}" = set; then
		enableval="$enable_static_devil"
		tcl_ok=$enableval
	else
		tcl_ok=no
	fi

	if test "$tcl_ok" = "yes" ; then
		AC_MSG_RESULT([static])
		TEA_ADD_LIBS([/usr/lib/libIL.a /usr/lib/libILU.a /usr/lib/libjpeg.a /usr/lib/libtiff.a /usr/lib/libpng.a /usr/lib/libmng.a /usr/lib/libz.a /usr/lib/liblcms.a])
	else
		AC_MSG_RESULT([shared])
		TEA_ADD_LIBS([-lIL -lILU])
	fi
])

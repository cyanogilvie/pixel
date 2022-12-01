#
# Include the TEA standard macro set
#

builtin(include,tclconfig/tcl.m4)

#
# Add here whatever m4 macros you want to define for your package
#

AC_DEFUN([CHECK_TESTMODE], [
	AC_MSG_CHECKING([whether to build in test mode])
	AC_ARG_ENABLE(testmode,
		[  --enable-testmode       Build in test mode (default: on)],
		[enable_testmode=$enableval],
		[enable_testmode="yes"])
	AC_MSG_RESULT($enable_testmode)
	if test "$enable_testmode" = "yes"
	then
		AC_DEFINE(TESTMODE)
	fi
])


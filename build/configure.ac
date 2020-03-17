AC_INIT([Storm], 0.2)
AM_INIT_AUTOMAKE([subdir-objects])

dnl add debug symbols to file
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [Enable debug, default:no]),
[case "${enableval}" in
	yes) debug=true ;;
	no) debug=false ;;
	*) AC_MSG_ERROR([Unknown value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

dnl force c++17
CXXFLAGS=" -std=c++17"

AC_PROG_CXX
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
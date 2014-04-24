AC_INIT([SoundCollection], [1], [jonathon@quotidian.org])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wno-portability no-define])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_MACRO_DIR([.])
AC_CONFIG_HEADER([config.h])
AC_PROG_CC()
AC_PROG_CXX()
AC_PROG_LIBTOOL()
AC_LANG([C++])

PKG_CHECK_MODULES([CORE],
                  [gom-1.0])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT()

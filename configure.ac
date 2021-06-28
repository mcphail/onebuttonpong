#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([pong], [0.3], [neil@themcphails.uk])
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIBM
AC_SUBST(LIBM)
PKG_CHECK_MODULES([SDL2], [sdl2 SDL2_image SDL2_mixer SDL2_ttf])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([atexit])

AC_CONFIG_FILES([Makefile
                 src/Makefile
		 res/Makefile])

CFLAGS="$CFLAGS --std=gnu99"

AC_OUTPUT

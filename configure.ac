

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.69])
AC_INIT([LLParse], [1.0], [BUG-REPORT-ADDRESS])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])
# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_AWK
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
  
m4_include([m4/ax_cxx_compile_stdcxx.m4])
AX_CXX_COMPILE_STDCXX(17, noext, mandatory)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([memset])

# Check for nessecary libraries
# CFLAGS="-std=iso9899:1990"
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([CppUTest/TestFilter.h],
                 [cpputest_found_headers=yes; break;])
AC_LANG_POP
AS_IF([test "x$cpputest_found_headers" != "xyes"],
        [AC_MSG_ERROR([Unable to find the cpputest headers])])

PKG_CHECK_MODULES([EXPAT], [expat])
PKG_CHECK_MODULES([CPPUTEST], [cpputest])

# Libtool init       
LT_PREREQ([2.4.2])
LT_INIT([disable-shared])

AC_CONFIG_FILES([
  Makefile
])

AC_OUTPUT



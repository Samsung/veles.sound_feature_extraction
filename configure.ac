dnl Process this file with autoconf to produce a configure script.

AC_INIT(spfextr, 1.0)
AC_PREREQ(2.68)
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_SYSTEM

AC_CONFIG_HEADERS([config.h])

AC_GNU_SOURCE
AC_C_BIGENDIAN
AC_PROG_CXX
AC_PROG_CC
AC_HEADER_STDC

dnl Initialize automake
AM_INIT_AUTOMAKE([1.11 dist-xz foreign color-tests])
dnl Nicely formatted make output
AM_SILENT_RULES([yes])

dnl This allows us specify individual liking flags for each target
AM_PROG_CC_C_O 

dnl Check for GCC version
AC_MSG_CHECKING([whether gcc version is >= 4.5.0])
AC_EGREP_CPP(passed, [
#ifdef __GNUC__
#if !(__GNUC__ < 4 || __GNUC_MINOR__ < 5 || __GNUC_PATCHLEVEL__ < 0)
passed
#endif
#endif
], [
	AC_MSG_RESULT([yes])
	AC_DEFINE([INLINE], [__attribute__((always_inline)) inline], [Set modifiers to always inline the code])
	AC_DEFINE([UNUSED], [__attribute__((unused))], [Mark parameters as unused to avoid warnings])
	AC_DEFINE([MALLOC], [__attribute__ ((__malloc__))], [malloc() function attribute])
	AC_DEFINE([WARN_UNUSED_RESULT], [__attribute__ ((__warn_unused_result__))],
		      [warn about unused result function attribute])
	AC_DEFINE([NOTNULL(params)], [__attribute__ ((__nonnull__ params))],
			  [Mark pointer parameters which must not be NULL])
], [
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Building this project requires GCC >= 4.5.0])
])

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries 
AC_PROG_LIBTOOL     

dnl Enable pkg-config      
PKG_PROG_PKG_CONFIG
EMBEDDED_LIBS=

dnl Check Zlib
PKG_CHECK_MODULES([ZLIB], [zlib >= 1.2.3])  

dnl Check for Doxygen
AC_ARG_ENABLE([doxygen],
	AS_HELP_STRING([--disable-doxygen], [Disable building HTML documentation with Doxygen])
)

AS_IF([test "x$enable_doxygen" != "xno"], [
	
	dnl If Doxygen is enabled...
	AC_PATH_PROG([DOXYGEN], [doxygen], "")
	AS_IF([test -z "$DOXYGEN"],
		AC_MSG_WARN([Doxygen was not found - continuing without Doxygen support])
	)
	
	AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
	AM_COND_IF([HAVE_DOXYGEN], [
		AC_CONFIG_FILES([docs/Doxyfile
						 docs/Makefile])
		DOCS_DIR=docs
		
		dnl Doxygen will require htags tools from GNU Global
		AC_PATH_PROG(GLOBAL, [htags], "")
		AS_IF([test -z "$GLOBAL"],
			AC_MSG_ERROR([Doxygen requires GNU GLOBAL to be installed])
		)
		
		dnl Doxygen will require dot tool from Graphviz
		AC_ARG_VAR(DOT, Path to Graphviz dot)		
		AC_PATH_PROG(DOT, [dot], "")
		AS_IF([test -z "$DOT"],
			AC_MSG_ERROR([Doxygen requires Graphviz to be installed])
		)
	])
],[
	AM_CONDITIONAL([HAVE_DOXYGEN], [false])
])
AC_SUBST([DOCS_DIR])

dnl Check for debug configuration
AC_ARG_ENABLE([debug],
              AS_HELP_STRING([--enable-debug], [Build in debug mode]),
              [DEBUG_FLAGS="-O0 -DDEBUG -Wextra" enable_debug=yes],
              [enable_debug=no])
AM_CONDITIONAL(DEBUG, test x$enable_debug = xyes)                                     

dnl Check whether to build tests
AC_ARG_ENABLE([tests],
	AS_HELP_STRING([--disable-tests], [Disable building tests])
)
AM_CONDITIONAL(TESTS, test x$enable_tests != xno)
AM_COND_IF([TESTS], [
	TESTS_DIR=tests
	AC_CONFIG_FILES(tests/Makefile
                	tests/google/Makefile
                	)

])
AC_SUBST([TESTS_DIR])

dnl Link with Makefile.am in source directories
AC_CONFIG_FILES(Makefile $PACKAGE.pc
				inc/Makefile
                src/Makefile         
                )           

dnl Security xFLAGS
SECURITYFLAGS="-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -Wall -Wno-comment -Wformat-security"
CXXFLAGS="$CXXFLAGS $SECURITYFLAGS"
CFLAGS="$CFLAGS $SECURITYFLAGS"
LDFLAGS="$LDFLAGS -Wl,-z,relro -Wl,--hash-style=gnu"

dnl Compile with -rdynamic
LDFLAGS="$LDFLAGS -rdynamic"

dnl Use the latest C++ standard 
CXXFLAGS="$CXXFLAGS -std=c++0x"

dnl Warn of unused precompiled headers
CXXFLAGS="$CXXFLAGS -Winvalid-pch"

dnl Use the modern C standard
CFLAGS="$CFLAGS -std=c99"

dnl Default visibility of symbols is hidden
CPPFLAGS="$CPPFLAGS -fvisibility=hidden"

dnl All include paths should be relative to source root or includes directory
CPPFLAGS="$CPPFLAGS -I\$(top_srcdir) -I\$(top_srcdir)/inc -I\$(top_builddir)/inc"
         
dnl Append debug flags             
CFLAGS="$CFLAGS $DEBUG_FLAGS"
CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS"         
                
AC_OUTPUT

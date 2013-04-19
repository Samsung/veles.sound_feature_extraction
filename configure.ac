dnl Process this file with autoconf to produce a configure script.

AC_INIT(SoundFeatureExtraction, 1.0)
AC_PREREQ(2.68)
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_SYSTEM

AC_CONFIG_HEADERS([src/config.h])

AC_ARG_VAR(SYSROOT, use this instead of compiler's default sysroot)
AS_IF([test "x$SYSROOT" != "x" ],	
	CPPFLAGS="$CPPFLAGS --sysroot=$SYSROOT"
	LDFLAGS="$LDFLAGS --sysroot=$SYSROOT"
)

AC_GNU_SOURCE
AC_PROG_CXX
AC_PROG_CC
AC_HEADER_STDC
AC_C_BIGENDIAN([AC_MSG_ERROR([Big endian build is not supported.])])

dnl Initialize automake
AM_INIT_AUTOMAKE([1.11 dist-xz foreign color-tests])
dnl Nicely formatted make output
AM_SILENT_RULES([yes])
dnl Rebuild everything be default
AM_MAINTAINER_MODE([enable])

dnl This allows us specify individual liking flags for each target
AM_PROG_CC_C_O

dnl Check for GCC version
AC_MSG_CHECKING([whether gcc version is >= 4.6.0])
AC_EGREP_CPP(passed, [
#ifdef __GNUC__
#if !(__GNUC__ < 4 || __GNUC_MINOR__ < 6 || __GNUC_PATCHLEVEL__ < 0)
passed
#endif
#endif
], [
	AC_MSG_RESULT([yes])
	AC_DEFINE([INLINE], [static __attribute__((always_inline)) inline], [Set modifiers to always inline the code])
	AC_DEFINE([UNUSED], [__attribute__((unused))], [Mark parameters as unused to avoid warnings])
	AC_DEFINE([MALLOC], [__attribute__ ((__malloc__))], [malloc() function attribute])
	AC_DEFINE([WARN_UNUSED_RESULT], [__attribute__ ((__warn_unused_result__))],
		      [warn about unused result function attribute])
	AC_DEFINE([NOTNULL(...)], [__attribute__ ((__nonnull__ (__VA_ARGS__)))],
			  [Mark pointer parameters which must not be NULL])
], [
	AC_MSG_RESULT([no])
	AC_MSG_ERROR([Building this project requires GCC compatible >= 4.5.0])
])

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries
AC_PROG_LIBTOOL

dnl Enable pkg-config
m4_ifndef([PKG_PROG_PKG_CONFIG], AC_MSG_ERROR([Configuration requires pkg-config to be installed]))
PKG_PROG_PKG_CONFIG

dnl Check whether to build subdirectories in parallel
AC_ARG_ENABLE([parallel_build],
	AS_HELP_STRING([--enable-parallel-build], [Enable building subdirectories in parallel])
)
AM_CONDITIONAL([PARALLEL_BUILD], [test "x$enable_parallel_build" == "xyes"])

dnl Use fancy make output by default
AC_ARG_ENABLE([colors],
	AS_HELP_STRING([--disable-colors], [Disable enhanced colored make output])
)
AS_IF([test "x$enable_colors" != "xno"], [
	am__v_CXX_0="@echo -e \"  \\033@<:@01;32m\$(CXX)\\033@<:@00m   \" \$@;"
	AC_SUBST([am__v_CXX_0])
	am__v_CC_0="@echo -e \"  \\033@<:@01;32m\$(CC)\\033@<:@00m   \" \$@;"
	AC_SUBST([am__v_CC_0])
	am__v_CXXLD_0="@echo -e \"  \\033@<:@01;36m\$(CXXLD)\\033@<:@00m   \" \$@;"
	AC_SUBST([am__v_CXXLD_0])
	am__v_CCLD_0="@echo -e \"  \\033@<:@01;36m\$(CCLD)\\033@<:@00m   \" \$@;"
	AC_SUBST([am__v_CCLD_0])
	am__v_GEN_0="@echo -e \"  \\033@<:@01;33mGEN\\033@<:@00m   \" \$@;"
	AC_SUBST([am__v_GEN_0])
])

dnl Check whether to use the built-in Boost
AC_ARG_WITH([built-in-boost],
	AS_HELP_STRING([--with-built-in-boost], [Use statically linked embedded Boost parts]), [
	BOOST_DIR="boost"
	LDFLAGS="$LDFLAGS -L\$(abs_top_builddir)/src/boost/stage/lib"
	dnl Set path to bjam
	AC_ARG_VAR(BJAM, path to bjam Boost builder)
	
	AC_PATH_PROG(BJAM, [bjam], "")
	
	AS_IF([test -z "$BJAM"],
		AC_MSG_ERROR([Building Boost requires a working bjam. You can obtain it via bootstrapping the Boost package.])
	)]
)
AC_SUBST([BOOST_DIR])

dnl Check whether to use the built-in FFTF
AC_ARG_WITH([built-in-fftf],
	AS_HELP_STRING([--with-built-in-fftf], [Use statically linked embedded FFTF]), [
	FFTF_LIBTOOL="\$(top_builddir)/fftf/src/libfftf.la"
	FFTF_DIR=fftf
	AC_CONFIG_SUBDIRS([fftf])
	CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/fftf/inc"
], [
	dnl Check for installed FFTF
	PKG_CHECK_MODULES([FFTF], [fftf >= 1.0])
])
AC_SUBST([FFTF_LIBTOOL])
AC_SUBST([FFTF_DIR])

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
		AC_ARG_VAR(DOT, path to Graphviz dot)		
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
AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [Build in debug mode]),
              [DEBUG_FLAGS="-O0 -DDEBUG -Wextra" enable_debug=yes],
              [enable_debug=no])
AM_CONDITIONAL(DEBUG, test "x$enable_debug" == "xyes")

dnl Enable/disable LTO
AC_ARG_ENABLE([lto],
	AS_HELP_STRING([--enable-lto], [Build with link-time optimizations (-flto)])
)

AS_IF([test "x$enable_lto" == "xyes"], [
	CPPFLAGS="$CPPFLAGS -flto" LDFLAGS="$LDFLAGS -flto"
])

dnl Check whether to build tests
AC_ARG_ENABLE([tests],
	AS_HELP_STRING([--disable-tests], [Disable building tests])
)
AM_CONDITIONAL(TESTS, test "x$enable_tests" != "xno")
AM_COND_IF([TESTS], [	
	TESTS_DIR=tests
	AC_CONFIG_FILES(tests/Makefile
                	tests/google/Makefile
                	tests/primitives/Makefile
                	tests/transforms/Makefile
                	)

], [
	dnl Default visibility of symbols is hidden
	CPPFLAGS="$CPPFLAGS -fvisibility=hidden"
])
AC_SUBST([TESTS_DIR])

dnl Check whether to conduct test benchmarks
AC_ARG_ENABLE([benchmarks],
	AS_HELP_STRING([--enable-benchmarks], [Enable executing SIMD speedup benchmarks during tests evaluation])
)
AS_IF([test "x$enable_benchmarks" == "xyes"], [
	CPPFLAGS="$CPPFLAGS -DBENCHMARK"
])

dnl Check whether to generate test coverage
AC_ARG_ENABLE([coverage],
	AS_HELP_STRING([--enable-test-coverage],
	               [Enable code coverage generation (this option depends on debug and tests switches)])
)
AS_IF([test "x$enable_test_coverage" == "xyes" -a "x$enable_debug" == "xyes" -a "x$enable_tests" != "xno"], [
	CPPFLAGS="$CPPFLAGS --coverage"
	LDFLAGS="$LDFLAGS -lgcov"
])

dnl Link with Makefile.am in source directories
AC_CONFIG_FILES(Makefile lib$PACKAGE_NAME.pc
				inc/Makefile
                src/Makefile
                src/boost/Makefile
                src/boost/user-config.jam
                src/formats/Makefile
                src/transforms/Makefile
                src/primitives/Makefile
                )

dnl Security xFLAGS
SECURITYFLAGS="-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2 -Wall -Wno-comment -Wformat-security"
CXXFLAGS="$CXXFLAGS $SECURITYFLAGS"
CFLAGS="$CFLAGS $SECURITYFLAGS"
LDFLAGS="$LDFLAGS -Wl,-z,relro -Wl,--hash-style=gnu"

dnl Compile with -rdynamic
LDFLAGS="$LDFLAGS -rdynamic"

dnl Use the latest C++ standard
CXXFLAGS="$CXXFLAGS -std=c++0x -fno-deduce-init-list"

dnl Warn of unused precompiled headers
CXXFLAGS="$CXXFLAGS -Winvalid-pch"

dnl Use the modern C standard
CFLAGS="$CFLAGS -std=c99"

dnl All include paths should be relative to source root or includes directory
CPPFLAGS="$CPPFLAGS -I\$(top_srcdir) -I\$(top_srcdir)/inc -I\$(top_builddir)"

dnl Append debug flags
CFLAGS="$CFLAGS $DEBUG_FLAGS"
CXXFLAGS="$CXXFLAGS $DEBUG_FLAGS"

dnl Use OpenMP
CPPFLAGS="$CPPFLAGS -fopenmp"

dnl Use the best march
AS_IF([test ${host%%-*} == i686 -o ${host%%-*} == x86_64], [
	CPPFLAGS="$CPPFLAGS -march=native"
])
AS_IF([test ${host%%-*} == arm], [
	CPPFLAGS="$CPPFLAGS -march=armv7-a -mfpu=neon"
])

AC_OUTPUT

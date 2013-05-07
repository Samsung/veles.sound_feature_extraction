dnl Process this file with autoconf to produce a configure script.

AC_INIT(SoundFeatureExtraction, 1.0)
m4_include([common.ac])

dnl Check whether to use the built-in Boost
AC_ARG_WITH([built-in-boost],
    AS_HELP_STRING([--with-built-in-boost], [use statically linked embedded Boost parts]), [
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
    AS_HELP_STRING([--with-built-in-fftf], [use statically linked embedded FFTF]), [
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

dnl Additional tests
AM_COND_IF([TESTS], [
    AC_CONFIG_FILES(tests/primitives/Makefile
                    tests/transforms/Makefile
                    )

])

dnl Check whether to conduct test benchmarks
AC_ARG_ENABLE([benchmarks],
    AS_HELP_STRING([--enable-benchmarks], [execute SIMD speedup benchmarks during tests evaluation])
)
AS_IF([test "x$enable_benchmarks" == "xyes"], [
    CPPFLAGS="$CPPFLAGS -DBENCHMARK"
])

dnl Link with Makefile.am in source directories
AC_CONFIG_FILES(src/boost/Makefile
                src/boost/user-config.jam
                src/formats/Makefile
                src/transforms/Makefile
                src/primitives/Makefile
                )

AC_OUTPUT

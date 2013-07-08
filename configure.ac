# Process this file with autoconf to produce a configure script.

AC_INIT(SoundFeatureExtraction, 1.0)
m4_include([common.ac])

# Check whether to conduct test benchmarks
AC_ARG_ENABLE([benchmarks],
    AS_HELP_STRING([--enable-benchmarks], [execute SIMD speedup benchmarks during tests evaluation])
)
AS_IF([test "x$enable_benchmarks" == "xyes"], [
    CPPFLAGS="$CPPFLAGS -DBENCHMARK"
])

# Check whether to use nice Eina logging
AC_ARG_ENABLE([eina-logging],
    AS_HELP_STRING([--enable-eina-logging], [Use Eina as the logging backend])
)
AS_IF([test "x$enable_eina_logging" == "xyes"], [    
    PKG_CHECK_MODULES([EINA], [eina >= 1.0])
    CPPFLAGS="$CPPFLAGS -DEINA $EINA_CFLAGS"
])

# Check whether to use the built-in Boost
AC_ARG_WITH([built-in-boost],
    AS_HELP_STRING([--with-built-in-boost], [use statically linked embedded Boost parts]), [
    BOOST_DIR="boost"
    LDFLAGS="$LDFLAGS -L\$(abs_top_builddir)/src/boost/stage/lib"
    # Set path to bjam
    AC_ARG_VAR(BJAM, path to bjam Boost builder)
    
    AC_PATH_PROG(BJAM, [bjam], "")
    
    AS_IF([test -z "$BJAM"],
        AC_MSG_ERROR([Building Boost requires a working bjam. You can obtain it via bootstrapping the Boost package.])
    )]
)
AC_SUBST([BOOST_DIR])

# Check whether to use the built-in FFTF
AC_ARG_WITH([built-in-fftf],
    AS_HELP_STRING([--with-built-in-fftf], [use bundled FFTF]), [
    FFTF_LIBS="\$(top_builddir)/fftf/src/libfftf.la"
    FFTF_DIR=fftf
    AC_CONFIG_SUBDIRS([fftf])
    CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/fftf/inc"
], [
    # Check for installed FFTF
    PKG_CHECK_MODULES([FFTF], [fftf >= 1.0])
])
AC_SUBST([FFTF_DIR])

# Check whether to use the built-in libSimd
AC_ARG_WITH([built-in-simd],
    AS_HELP_STRING([--with-built-in-simd], [use bundled libSimd]), [
    SIMD_LIBS="\$(top_builddir)/simd/src/libSimd.la"
    SIMD_DIR=simd
    echo "../fftf" > simd/built-in-fftf
    AC_CONFIG_SUBDIRS([simd])
    AS_IF([test "x$with_built_in_fftf" == xyes], [
    	AC_CONFIG_LINKS(["$top_builddir""simd/src/fftf":"$top_srcdir""fftf/inc/fftf"])
    ])
    CPPFLAGS="$CPPFLAGS -I\$(top_srcdir)/simd/inc"    
], [
    # Check for installed SIMD
    PKG_CHECK_MODULES([SIMD], [Simd >= 1.0])
])
AC_SUBST([SIMD_DIR])

# Additional tests
AM_COND_IF([TESTS], [
    AC_CONFIG_FILES(tests/primitives/Makefile
                    tests/transforms/Makefile
                    tests/allocators/Makefile
                    )
])

# Link with Makefile.am in additional source directories
AC_CONFIG_FILES(src/boost/Makefile
                src/boost/user-config.jam
                src/formats/Makefile
                src/transforms/Makefile
                src/primitives/Makefile
                src/allocators/Makefile
                )

AC_OUTPUT

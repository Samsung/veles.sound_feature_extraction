include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = arithmetic convolute window

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
arithmetic_SOURCES = arithmetic.cc

# Compiler options
arithmetic_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
arithmetic_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
convolute_SOURCES = convolute.cc

# Compiler options
convolute_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
convolute_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
window_SOURCES = window.cc

# Compiler options
window_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
window_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

include $(top_srcdir)/tests/Tests.make

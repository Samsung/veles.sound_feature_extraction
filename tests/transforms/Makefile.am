include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = hamming_window_filter lowpass_filter

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
hamming_window_filter_SOURCES = hamming_window_filter.cc

# Compiler options
hamming_window_filter_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
hamming_window_filter_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
lowpass_filter_SOURCES = lowpass_filter.cc

# Compiler options
lowpass_filter_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
lowpass_filter_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

include $(top_srcdir)/tests/Tests.make

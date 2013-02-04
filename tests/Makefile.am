include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = speech_features

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
speech_features_SOURCES = speech_features.cc

# Compiler options
speech_features_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
speech_features_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

SUBDIRS = google

include $(top_srcdir)/tests/Tests.make

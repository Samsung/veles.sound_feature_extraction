include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = speech_features parameters transform_tree mfcc

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
speech_features_SOURCES = speech_features.cc

# Compiler options
speech_features_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
speech_features_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
transform_tree_SOURCES = transform_tree.cc

# Compiler options
transform_tree_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
transform_tree_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
parameters_SOURCES = parameters.cc

# Compiler options
parameters_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
parameters_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
mfcc_SOURCES = mfcc.cc

# Compiler options
mfcc_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
mfcc_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

SUBDIRS = google primitives transforms

include $(top_srcdir)/tests/Tests.make

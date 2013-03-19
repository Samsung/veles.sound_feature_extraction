include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = features_parser parameters transform_tree mfcc sbc

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
features_parser_SOURCES = features_parser.cc

# Compiler options
features_parser_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
features_parser_LDADD = $(top_builddir)/src/libspfextr.la  \
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

# Sources 
sbc_SOURCES = sbc.cc

# Compiler options
sbc_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
sbc_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

SUBDIRS = google primitives transforms

include $(top_srcdir)/tests/Tests.make

include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = lowpass_filter square magnitude log filter_bank

GTEST_INCLUDE = -I$(top_srcdir)/tests/google

# Sources 
lowpass_filter_SOURCES = lowpass_filter.cc

# Compiler options
lowpass_filter_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
lowpass_filter_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
square_SOURCES = square.cc

# Compiler options
square_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
square_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
magnitude_SOURCES = magnitude.cc

# Compiler options
magnitude_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
magnitude_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
log_SOURCES = log.cc

# Compiler options
log_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
log_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
filter_bank_SOURCES = filter_bank.cc

# Compiler options
filter_bank_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
filter_bank_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

include $(top_srcdir)/tests/Tests.make

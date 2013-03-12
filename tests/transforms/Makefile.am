include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = lowpass_filter square complex_magnitude log filter_bank \
energy real_to_complex unpack_rdft intensity

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
complex_magnitude_SOURCES = complex_magnitude.cc

# Compiler options
complex_magnitude_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
complex_magnitude_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
energy_SOURCES = energy.cc

# Compiler options
energy_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
energy_LDADD = $(top_builddir)/src/libspfextr.la  \
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

# Sources 
real_to_complex_SOURCES = real_to_complex.cc

# Compiler options
real_to_complex_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
real_to_complex_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
unpack_rdft_SOURCES = unpack_rdft.cc

# Compiler options
unpack_rdft_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
unpack_rdft_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
intensity_SOURCES = intensity.cc

# Compiler options
intensity_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
intensity_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

include $(top_srcdir)/tests/Tests.make

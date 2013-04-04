include $(top_srcdir)/Makefile.common

noinst_PROGRAMS = lowpass_filter square complex_magnitude log filter_bank \
energy real_to_complex unpack_rdft intensity rdft dct dwpt subband_energy \
zero_padding selector hps window

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

# Sources 
rdft_SOURCES = rdft.cc

# Compiler options
rdft_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
rdft_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
dct_SOURCES = dct.cc

# Compiler options
dct_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
dct_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
dwpt_SOURCES = dwpt.cc

# Compiler options
dwpt_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
dwpt_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
subband_energy_SOURCES = subband_energy.cc

# Compiler options
subband_energy_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
subband_energy_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
zero_padding_SOURCES = zero_padding.cc

# Compiler options
zero_padding_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
zero_padding_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
selector_SOURCES = selector.cc

# Compiler options
selector_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
selector_LDADD = $(top_builddir)/src/libspfextr.la  \
$(top_builddir)/tests/google/lib_gtest.la \
$(ALL_LIBS)

# Sources 
hps_SOURCES = hps.cc

# Compiler options
hps_CPPFLAGS = $(GTEST_INCLUDE)

# Used libraries
hps_LDADD = $(top_builddir)/src/libspfextr.la  \
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

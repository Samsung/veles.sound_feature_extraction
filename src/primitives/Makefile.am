include $(top_srcdir)/Makefile.common

noinst_LTLIBRARIES = lib_primitives.la

lib_primitives_la_SOURCES = memory.c convolute.c avx_extra.c daubechies.c wavelet.c window.cc \
wavelet_filter_bank.cc energy.c coiflets.c symlets.c

lib_primitives_la_CPPFLAGS = @FFTF_CFLAGS@
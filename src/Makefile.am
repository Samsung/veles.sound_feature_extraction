include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libSoundFeatureExtraction.la
noinst_LTLIBRARIES = libDSPFilters.la

#######################################
# Build information for libSoundFeatureExtraction.la

libSoundFeatureExtraction_la_SOURCES = api.cc buffers.cc buffer_format.cc \
features_parser.cc parameterizable.cc transform.cc transform_registry.cc \
transform_tree.cc format_converter.cc demangle.cc parameterizable_base.cc \
logger.cc simd_aware.cc \
\
allocators/sliding_blocks_allocator.cc allocators/worst_allocator.cc \
allocators/buffers_allocator.cc allocators/sliding_blocks_impl.cc \
\
formats/int16_to_int32.cc formats/int32_to_int16.cc formats/int16_to_float.cc \
formats/float_to_int16.cc formats/int32_to_float.cc formats/float_to_int32.cc \
formats/single_converters.cc \
\
primitives/window.cc primitives/wavelet_filter_bank.cc primitives/energy.c \
primitives/lpc.c primitives/lsp.c \
\
transforms/window.cc transforms/lowpass_filter.cc \
transforms/highpass_filter.cc transforms/bandpass_filter.cc \
transforms/convolve.cc transforms/fir_filter_base.cc transforms/square.cc \
transforms/rdft.cc transforms/dct.cc transforms/complex_magnitude.cc \
transforms/spectral_energy.cc transforms/filter_bank.cc \
transforms/log.cc transforms/real_to_complex.cc transforms/unpack_rdft.cc \
transforms/dwpt.cc transforms/subband_energy.cc \
transforms/zero_padding.cc transforms/selector.cc transforms/shc.cc \
transforms/fork.cc transforms/complex_to_real.cc transforms/rectify.cc \
transforms/frequency_bands.cc transforms/diff.cc transforms/mean.cc \
transforms/sfm.cc transforms/energy.cc transforms/subsampling.cc \
transforms/centroid.cc transforms/zerocrossings.cc transforms/rolloff.cc \
transforms/flux.cc transforms/beat.cc transforms/autocorrelation.cc \
transforms/delta.cc transforms/short_time_msn.cc transforms/preemphasis.cc \
transforms/stats.cc transforms/window_splitter.cc transforms/common.cc \
transforms/single_to_raw.cc transforms/iir_filter_base.cc \
transforms/mix_stereo.cc transforms/peak_detection.cc transforms/identity.cc \
transforms/peak_analysis.cc transforms/peak_dynamic_programming.cc \
transforms/lpc.cc transforms/lsp.cc transforms/lpc_cc.cc

libSoundFeatureExtraction_la_LIBADD = @SIMD_LIBS@ @FFTF_LIBS@ -lboost_regex \
	@EINA_LIBS@ libDSPFilters.la
	
libSoundFeatureExtraction_la_LDFLAGS = $(AM_LDFLAGS) \
	-version-info $(INTERFACE_VERSION):$(REVISION_NUMBER):$(AGE_NUMBER)

#######################################
# Build information for libDSPFilters.la

libDSPFilters_la_SOURCES =../DSPFilters/shared/DSPFilters/source/Bessel.cpp \
../DSPFilters/shared/DSPFilters/source/Biquad.cpp \
../DSPFilters/shared/DSPFilters/source/Butterworth.cpp \
../DSPFilters/shared/DSPFilters/source/Cascade.cpp \
../DSPFilters/shared/DSPFilters/source/ChebyshevI.cpp \
../DSPFilters/shared/DSPFilters/source/ChebyshevII.cpp \
../DSPFilters/shared/DSPFilters/source/Custom.cpp \
../DSPFilters/shared/DSPFilters/source/Design.cpp \
../DSPFilters/shared/DSPFilters/source/Elliptic.cpp \
../DSPFilters/shared/DSPFilters/source/Filter.cpp \
../DSPFilters/shared/DSPFilters/source/Legendre.cpp \
../DSPFilters/shared/DSPFilters/source/Param.cpp \
../DSPFilters/shared/DSPFilters/source/PoleFilter.cpp \
../DSPFilters/shared/DSPFilters/source/RBJ.cpp \
../DSPFilters/shared/DSPFilters/source/RootFinder.cpp \
../DSPFilters/shared/DSPFilters/source/State.cpp

libDSPFilters_la_CXXFLAGS = $(AM_CXXFLAGS) -Wno-old-style-cast \
	-Wno-unused-parameter -Wno-ignored-qualifiers -Wno-unused-but-set-variable \
	-Wno-sequence-point

PARALLEL_SUBDIRS = $(BOOST_DIR)

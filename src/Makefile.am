include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libSoundFeatureExtraction.la

#######################################
# Build information for libSoundFeatureExtraction.la

# Source files
libSoundFeatureExtraction_la_SOURCES = api.cc buffers.cc buffer_format.cc \
features_parser.cc parameterizable.cc transform.cc transform_registry.cc \
transform_tree.cc format_converter.cc demangle.cc parameters_base.cc logger.cc \
simd_aware.cc \
\
allocators/sliding_blocks_allocator.cc allocators/worst_allocator.cc \
allocators/buffers_allocator.cc allocators/sliding_blocks_impl.cc \
\
formats/int16_to_int32.cc formats/int32_to_int16.cc formats/int16_to_float.cc \
formats/float_to_int16.cc formats/int32_to_float.cc formats/float_to_int32.cc \
formats/single_converters.cc \
\
primitives/window.cc primitives/wavelet_filter_bank.cc primitives/energy.c \
\
transforms/window.cc transforms/lowpass_filter.cc \
transforms/highpass_filter.cc transforms/bandpass_filter.cc \
transforms/convolve.cc transforms/fir_filter_base.cc transforms/square.cc \
transforms/rdft.cc transforms/dct.cc transforms/complex_magnitude.cc \
transforms/spectral_energy.cc transforms/filter_bank.cc \
transforms/log.cc transforms/real_to_complex.cc transforms/unpack_rdft.cc \
transforms/intensity.cc transforms/dwpt.cc transforms/subband_energy.cc \
transforms/zero_padding.cc transforms/selector.cc transforms/hps.cc \
transforms/fork.cc transforms/complex_to_real.cc transforms/rectify.cc \
transforms/frequency_bands.cc transforms/diffrect.cc transforms/mean.cc \
transforms/sfm.cc transforms/argminmax.cc transforms/energy.cc \
transforms/centroid.cc transforms/zerocrossings.cc transforms/rolloff.cc \
transforms/flux.cc transforms/beat.cc transforms/autocorrelation.cc \
transforms/delta.cc transforms/short_time_msn.cc transforms/preemphasis.cc \
transforms/stats.cc transforms/window_splitter.cc transforms/common.cc

# Used libraries
libSoundFeatureExtraction_la_LIBADD = @SIMD_LIBS@ @FFTF_LIBS@ -lboost_regex \
	@EINA_LIBS@
# libSoundFeatureExtraction_la_LIBADD = transforms/lib_transforms.la \
# formats/lib_formats.la primitives/lib_primitives.la allocators/lib_allocators.la \
# @SIMD_LIBS@ @FFTF_LIBS@ -lboost_regex @EINA_LIBS@

#PARALLEL_SUBDIRS = $(BOOST_DIR) formats transforms primitives allocators
PARALLEL_SUBDIRS = $(BOOST_DIR)

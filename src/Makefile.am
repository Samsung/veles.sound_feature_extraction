include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libSoundFeatureExtraction.la

#######################################
# Build information for libSoundFeatureExtraction.la

# Source files
libSoundFeatureExtraction_la_SOURCES = api.cc buffers.cc buffer_format.cc features_parser.cc \
parameterizable.cc transform.cc transform_registry.cc transform_tree.cc \
format_converter.cc demangle.cc parameters_base.cc logger.cc

# Used libraries
libSoundFeatureExtraction_la_LIBADD = transforms/lib_transforms.la formats/lib_formats.la \
primitives/lib_primitives.la $(FFTF_LIBTOOL) @FFTF_LIBS@ $(SIMD_LIBTOOL) @SIMD_LIBS@ \
-lboost_regex @EINA_LIBS@

PARALLEL_SUBDIRS = $(BOOST_DIR) transforms formats primitives

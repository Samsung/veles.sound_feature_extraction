include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libspfextr.la

#######################################
# Build information for libspfextr.la

# Source files
libspfextr_la_SOURCES = api.cc buffers.cc buffer_format.cc features_parser.cc \
parameters.cc transform.cc transform_registry.cc transform_tree.cc \
format_converter.cc demangle.cc parameters_base.cc

# Linker options
libspfextr_la_LDFLAGS = 

# Compiler options
libspfextr_la_CPPFLAGS =

# Used libraries
libspfextr_la_LIBADD = transforms/lib_transforms.la formats/lib_formats.la \
primitives/lib_primitives.la $(FFTF_LIBTOOL) @FFTF_LIBS@ -lboost_regex

SUBDIRS = $(BOOST_DIR) transforms formats primitives

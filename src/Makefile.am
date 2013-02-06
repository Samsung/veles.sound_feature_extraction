include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libspfextr.la

#######################################
# Build information for libspfextr.la

# Source files
libspfextr_la_SOURCES = api.cc buffers.cc buffer_format.cc speech_features.cc \
parameters.cc transform.cc transform_registry.cc transform_tree.cc

# Linker options
libspfextr_la_LDFLAGS = 

# Compiler options
libspfextr_la_CPPFLAGS =

# Used libraries
libspfextr_la_LIBADD = -lboost_regex transforms/lib_transforms.la formats/lib_formats.la

SUBDIRS = $(BOOST_DIR) transforms formats

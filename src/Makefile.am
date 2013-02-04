include $(top_srcdir)/Makefile.common

lib_LTLIBRARIES = libspfextr.la

#######################################
# Build information for libspfextr.la

# Source files
libspfextr_la_SOURCES = api.cc buffers.cc parameters.cc transform.cc transform_registry.cc transform_tree.cc

# Linker options
libspfextr_la_LDFLAGS = 

# Compiler options
libspfextr_la_CPPFLAGS = 

# Used libraries
libspfextr_la_LIBADD = 

SUBDIRS =


#######################################
# Subdirectory instructions

# 1. Create Makefile.am in the subdirectory:

#include $(top_srcdir)/Makefile.common

#noinst_LTLIBRARIES = lib_subdir.la

# Sources
#lib_subdir_la_SOURCES =

# Compiler options
#lib_subdir_la_CPPFLAGS =

# 2. Add subdir/Makefile to configure.ac at AC_CONFIG_FILES

# 3. Add reference to lib_subdir.la in libspfextr.la:

# libspfextr_la_LIBADD = subdir/lib_subdir.la

# 4. Add subdir to parent directory Makefile.am's SUBDIRS

# SUBDIRS = subdir

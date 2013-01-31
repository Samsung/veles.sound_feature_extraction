#######################################
# We are building the static library
# lib_gtest.a with libtool
noinst_LTLIBRARIES = lib_gtest.la

#######################################
# Build information for lib_gtest.la

# Sources for lib_gtest.a
lib_gtest_la_SOURCES = gtest-all.cc

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
lib_gtest_la_CPPFLAGS = -I$(top_srcdir)/tests/google
CXXFLAGS = -O2 -g

.PHONY: tests

tests:
	@echo >/dev/null
SUBDIRS=$(FFTF_DIR) $(SIMD_DIR) inc src $(TESTS_DIR) $(DOCS_DIR)
DIST_SUBDIRS=$(FFTF_DIR) $(SIMD_DIR) inc src $(TESTS_DIR) $(DOCS_DIR)
DISTCHECK_CONFIGURE_FLAGS = --disable-doxygen

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc

.PHONY : tests

export TESTLOG ?= tests.log

tests: all
	@rm -f $(TESTLOG) 
	@cd tests; $(MAKE) tests
	@if [ -n "$$(grep '\[FAILED\]' $$TESTLOG)" ]; then \
		echo "One or more tests failed"; \
		exit 1; \
	fi

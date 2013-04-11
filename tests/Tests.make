if PARALLEL_BUILD

$(PARALLEL_SUBDIRS)::
	$(MAKE) -C $@ $(MAKECMDGOALS)

all-local:: $(PARALLEL_SUBDIRS)

clean-local:: $(PARALLEL_SUBDIRS)
	rm -f *.xml

SUBDIRS = $(DEPENDENCY_SUBDIRS)

else

SUBDIRS = $(DEPENDENCY_SUBDIRS) $(PARALLEL_SUBDIRS)

clean-local::
	rm -f *.xml
	
endif

AM_DEFAULT_SOURCE_EXT = .cc

AM_CPPFLAGS = -I$(top_srcdir)/tests/google
AM_LDFLAGS = $(top_builddir)/src/libSoundFeatureExtraction.la \
       $(top_builddir)/tests/google/lib_gtest.la

noinst_PROGRAMS = $(TESTS)

.PHONY: tests

REALLOG=$(top_builddir)/$(TESTLOG)

DEFAULT_TIMEOUT=10

tests:
	@for dir in $(PARALLEL_SUBDIRS); do \
		cd $$dir; $(MAKE) --no-print-directory tests; cd ..; \
	done
	@echo [~~~~~~~~~~] >>$(REALLOG)
	@echo Running tests in $(srcdir) >>$(REALLOG)
	@echo [~~~~~~~~~~] >>$(REALLOG)
	@for et in $(TESTS); do \
	skip=0; \
	for nt in $(not_tests); do \
		if [ "$$et" == "$$nt" ]; then \
			skip=1; \
			break; \
		fi; \
	done; \
	if [ "$$skip" == "1" ]; then \
		continue; \
	fi; \
	if [ -z "$(TIMEOUT)" ]; then \
		timeout $(DEFAULT_TIMEOUT) ./$$et --gtest_output="xml:$$et.xml" &>>$(REALLOG); \
	else \
		timeout $(TIMEOUT) ./$$et --gtest_output="xml:$$et.xml" &>>$(REALLOG); \
	fi; \
	if [ "$$?" -eq "0" ]; then \
		echo -e "\033[01;32m[DONE]\033[00m $$et"; \
	else \
		echo -e "\033[01;31m[FAIL]\033[00m $$et"; \
		echo "[FAILED]" >>$(REALLOG); \
	fi; \
	done

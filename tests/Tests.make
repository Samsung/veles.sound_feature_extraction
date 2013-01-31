TESTS = $(noinst_PROGRAMS)

.PHONY: tests

REALLOG=$(top_builddir)/$(TESTLOG)

DEFAULT_TIMEOUT=5

tests:	
	@for dir in $(SUBDIRS); do \
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
		timeout $(DEFAULT_TIMEOUT) ./$$et &>>$(REALLOG); \
	else \
		timeout $(TIMEOUT) ./$$et &>>$(REALLOG); \
	fi; \
	if [ "$$?" -eq "0" ]; then \
		echo -e "\033[01;32m[DONE]\033[00m $$et"; \
	else \
		echo -e "\033[01;31m[FAIL]\033[00m $$et"; \
	fi; \
	done
	
ALL_LIBS = @ZLIB_LIBS@	

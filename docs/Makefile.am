if HAVE_DOXYGEN
directory = $(top_builddir)/docs/html 

$(directory)index.html: doxyfile.stamp

doxyfile.stamp: $(top_srcdir)/inc/sound_feature_extraction/api.h Doxyfile
	$(AM_V_GEN)$(DOXYGEN) Doxyfile
	$(AM_V_at)echo Timestamp > doxyfile.stamp

CLEANFILES = doxyfile.stamp

all-local: $(directory)index.html doxyfile.stamp

install-data-local: doxyfile.stamp
	$(AM_V_GEN)$(MKDIR_P) $(DESTDIR)$(htmldir)/search && \
	for file in $$(find $(top_builddir)/docs/html -maxdepth 1 -type f); do \
		$(INSTALL_DATA) $$file $(DESTDIR)$(htmldir); \
	done && \
	for file in $$(find $(top_builddir)/docs/html/search -maxdepth 1 -type f); do \
		$(INSTALL_DATA) $$file $(DESTDIR)$(htmldir)/search; \
	done

uninstall-local:
	$(AM_V_at)rm -rf $(DESTDIR)$(htmldir)

clean-local:
	$(AM_V_at)rm -rf $(directory)
	
endif

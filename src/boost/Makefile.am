bjam.stamp:
	$(AM_V_GEN)$(BJAM) --toolset=gcc --build-dir="$(abs_builddir)/libs" --stagedir="$(abs_builddir)/stage" \
	-j$$(getconf _NPROCESSORS_ONLN) link=static cxxflags="$(CXXFLAGS) -fPIC" "$(srcdir)"
	$(AM_V_at)echo stamp > bjam.stamp

CLEANFILES = bjam.stamp

all-local: bjam.stamp	

install-data-local: bjam.stamp

uninstall-local:

clean-local:
	$(AM_V_at)rm -rf $(builddir)/libs $(builddir)/stage
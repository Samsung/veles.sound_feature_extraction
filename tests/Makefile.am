TESTS = features_parser parameters transform_tree mfcc sbc wpp api sfm vad \
musical_surface all_features

api_LDADD = -lboost_regex
all_features_LDADD = $(FFTF_LIBTOOL) @FFTF_LIBS@

PARALLEL_SUBDIRS = primitives transforms

DEPENDENCY_SUBDIRS = google

include $(top_srcdir)/tests/Tests.make

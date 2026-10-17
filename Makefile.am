ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libhep.la
libhep_la_SOURCES = \
	core_hep.c \
	src/hep_log.c \
	src/hep_io.c \
	src/hep_config.c \
	src/hep_proto.c \
	src/hep_transport.c

libhep_la_CPPFLAGS = -I$(top_srcdir)/include
libhep_la_CFLAGS = $(ZLIB_CFLAGS) -Wall
libhep_la_LIBADD = $(ZLIB_LIBS) -lpthread
libhep_la_LDFLAGS = -version-info 1:0:0

hepincludedir = $(includedir)/hep
hepinclude_HEADERS = \
	include/hep/hep_api.h \
	include/hep/hep_config.h \
	include/hep/hep_io.h \
	include/hep/hep_log.h \
	include/hep/hep_portability.h \
	include/hep/hep_proto.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libhep.pc

check_PROGRAMS = tests/test_hep_auth_transport
TESTS = $(check_PROGRAMS)

tests_test_hep_auth_transport_SOURCES = tests/test_hep_auth_transport.c
tests_test_hep_auth_transport_CPPFLAGS = -I$(top_srcdir)/include
tests_test_hep_auth_transport_LDADD = libhep.la

EXTRA_DIST = core_hep.h docs/TRANSPORT.md

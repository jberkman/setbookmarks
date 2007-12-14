# -*- Makefile -*-

bin_PROGRAMS += setbookmarks

setbookmarks_SOURCES := setbookmarks.c

setbookmarks_CFLAGS := \
    -DPACKAGE_DATA_DIR=\""$(datadir)"\"			       \
    -DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
    $(PACKAGE_CFLAGS)

setbookmarks_LDADD := $(PACKAGE_LIBS) $(INTLLIBS)

apps_DATA += setbookmarks.desktop

dist_noinst_DATA +=				\
	autogen.sh				\
	bookmark-agent-libslab.h		\
	intltool-extract.in			\
	intltool-merge.in			\
	intltool-update.in			\
	libslab-bookmarkfile.c			\
	libslab-bookmarkfile.h			\
	setbookmarks.desktop.in			\
	setbookmarks.spec

DISTCLEANFILES =				\
	intltool-extract			\
	intltool-merge				\
	intltool-update

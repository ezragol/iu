SUBDIRS = FileSystem Options Socket Actions

bin_PROGRAMS = iu
iu_SOURCES = main.c

iu_CFLAGS = -I$(srcdir)/FileSystem -I$(srcdir)/Options -I$(srcdir)/Socket -I$(srcdir)/Actions
iu_LDADD = FileSystem/libFileSystem.a Options/libOptions.a Actions/libActions.a Socket/libSocket.a
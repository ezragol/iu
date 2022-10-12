SUBDIRS = FileSystem Options Socket Headers Actions

bin_PROGRAMS = iu
iu_SOURCES = main.c

iu_CFLAGS = -I$(srcdir)/FileSystem -I$(srcdir)/Options -I$(srcdir)/Socket -I$(srcdir)/Header -I$(srcdir)/Actions
iu_LDADD = FileSystem/libFileSystem.a Options/libOptions.a Socket/libSocket.a Headers/libHeaders.a Actions/libActions.a -lm 
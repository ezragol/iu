SUBDIRS = FileSystem Options Socket Actions

bin_PROGRAMS = iu
iu_SOURCES = \
    hashmap.c \
    hashmap.h \
    main.c

iu_CFLAGS = -I$(srcdir)/FileSystem -I$(srcdir)/Options -I$(srcdir)/Actions -I$(srcdir)/Socket
iu_LDADD = FileSystem/libFileSystem.a Options/libOptions.a Socket/libSocket.a Actions/libActions.a 
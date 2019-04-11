CFLAGS?=-Os -g -Wall
LDFLAGS?=

GLIB_CFLAGS:=$(shell pkg-config --cflags gio-2.0 json-glib-1.0)
GLIB_LIBS:=$(shell pkg-config --libs gio-2.0 json-glib-1.0)

all: desktopgen

desktopgen: desktopgen.o
	$(CC) $(LDFLAGS) -o $@ $^ $(GLIB_LIBS) $(LIBS)

%.o: %.c
	$(CC) -std=gnu99 $(GLIB_CFLAGS) $(CFLAGS) -o $@ -c $<

install: all
	install $(IFLAGS) desktopgen $(DESTDIR)/bin

clean:
	rm -f desktopgen *.o *.core

.PHONY: all clean

CFLAGS?=-Os -g -Wall
LDFLAGS?=

all: libstub.so

libstub.so: stub.o
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) -fPIC -DPIC $(CFLAGS) -o $@ -c $<

install: all
	install $(IFLAGS) libstub.so $(DESTDIR)/lib

clean:
	rm -f libstub.so *.o

.PHONY: all clean

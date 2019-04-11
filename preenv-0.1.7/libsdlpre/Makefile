CFLAGS?=-Os -g -Wall

SDL_CFLAGS:=$(shell sdl-config --cflags)
SDL_LIBS:=$(shell sdl-config --libs)  -lSDL_gles
#//k PKGCONFIG_PKGS:=libosso libhildonmime liblocation gstreamer-plugins-base-0.10 gstreamer-interfaces-0.10 x11 mce
PKGCONFIG_PKGS:=dbus-1 dbus-glib-1 glib-2.0 gstreamer-plugins-base-0.10 gstreamer-interfaces-0.10 x11 mce libosso
PKGCONFIG_CFLAGS:=$(shell pkg-config $(PKGCONFIG_PKGS) --cflags)
PKGCONFIG_LIBS:=$(shell pkg-config $(PKGCONFIG_PKGS) --libs)
DL_LIBS:=-ldl

OBJ:=private.o pdl.o sdlgl.o sdl.o joystick.o haptic.o cinema.o x11.o taskbtn.o gles1.o

all: libsdlpre.so

libsdlpre.so: $(OBJ)
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(SDL_LIBS) $(PKGCONFIG_LIBS) $(DL_LIBS)

$(OBJ): %.o: %.c
	$(CC) -fPIC -DPIC $(SDL_CFLAGS) $(PKGCONFIG_CFLAGS) $(CFLAGS) -I. -o $@ -c $<

install: libsdlpre.so
	install $(IFLAGS) libsdlpre.so $(DESTDIR)/lib/

clean: 
	rm -f libsdlpre.so *.o

.PHONY: all install clean


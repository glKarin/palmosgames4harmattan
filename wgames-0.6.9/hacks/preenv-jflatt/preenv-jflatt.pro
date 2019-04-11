QT =
TEMPLATE = lib
CONFIG += plugin link_pkgconfig
TARGET = sdlpre
#k PKGCONFIG += libosso libhildonmime liblocation gstreamer-plugins-base-0.10 gstreamer-interfaces-0.10 x11 mce sdl
PKGCONFIG += dbus-1 dbus-glib-1 glib-2.0 gconf-2.0 gstreamer-plugins-base-0.10 gstreamer-interfaces-0.10 x11 mce sdl libosso
SOURCES += private.c pdl.c sdl.c joystick.c haptic.c cinema.c x11.c gles1.c sdlgl.c
LIBS += -ldl #-lSDL_gles
CFLAGS += -Os -g

SOURCES += sdlgl/SDL_gles.c
HEADERS += sdlgl/SDL_gles.h sdlgl/attribs.inc
INCLUDEPATH += sdlgl
LIBS += -lEGL

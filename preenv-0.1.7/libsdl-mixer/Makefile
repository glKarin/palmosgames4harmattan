FETCH_URL:=http://palm.cdnetworks.net/opensource/1.4.5/libsdl-mixer-1.2.tgz
PATCH_URL:=http://palm.cdnetworks.net/opensource/1.4.5/libsdl-mixer-1.2-patch.gz
DIR:=libsdl-mixer-1.2

FETCH_FILE:=$(notdir $(FETCH_URL))
PATCH_FILE:=$(notdir $(PATCH_URL))

all: build

fetch: $(FETCH_FILE) $(PATCH_FILE)

$(FETCH_FILE):
	wget -O "$@" "$(FETCH_URL)"

$(PATCH_FILE):
	wget -O "$@" "$(PATCH_URL)"

unpack: $(DIR)

$(DIR): $(FETCH_FILE) $(PATCH_FILE)
	tar zxf $<
	gunzip -c $(PATCH_FILE) | patch -p0 -t -d "$(DIR)"
	patch -p1 -t -d "$(DIR)" < fix-linking.patch

$(DIR)/config.status:
	cd "$(DIR)" && ./configure --prefix=/usr -disable-sdltest \
		--enable-music-ogg-tremor --disable-music-mp3 --disable-music-flac \
		CFLAGS="$(CFLAGS) -I$(CURDIR)/../ffmpeg/root/usr/include" \
		LDFLAGS="$(LDFLAGS) -L$(CURDIR)/../ffmpeg/root/usr/lib -Wl,-rpath-link -Wl,$(CURDIR)/../faac/root/usr/lib -Wl,-rpath-link -Wl,$(CURDIR)/../opencore-amr/root/usr/lib"

build: $(DIR) $(DIR)/config.status
	$(MAKE) -C "$(DIR)" all
	mkdir -p root
	$(MAKE) -C "$(DIR)" install DESTDIR="$(CURDIR)/root"

install: build
	cp -rv root/usr/lib/libSDL_mixer-1.2.so* $(DESTDIR)/lib

clean:
	rm -rf "$(DIR)" root

.PHONY: all unpack configure build install clean


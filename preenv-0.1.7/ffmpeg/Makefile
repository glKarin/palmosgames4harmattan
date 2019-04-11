FETCH_URL:=http://palm.cdnetworks.net/opensource/2.1.0/ffmpeg-0.6.tar.gz
PATCH_URL:=http://palm.cdnetworks.net/opensource/2.1.0/ffmpeg-0.6-patches.tgz
DIR:=ffmpeg-0.6

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
	gunzip -c $(PATCH_FILE) | patch -p1 -t -d "$(DIR)"

$(DIR)/config.mak:
	cd "$(DIR)" && ./configure --prefix=/usr \
	--disable-bsfs --disable-decoders --disable-demuxers --disable-devices \
	--disable-encoders --disable-ffplay --disable-ffserver --disable-filters \
	--disable-indevs --disable-mpegaudio-hp --disable-muxers --disable-network \
	--disable-outdevs --disable-parsers --disable-protocols --disable-debug \
	--enable-pthreads --enable-shared \
	--extra-cflags="$(CFLAGS) -I$(CURDIR)/../faac/root/usr/include -I$(CURDIR)/../opencore-amr/root/usr/include" \
	--extra-ldflags="$(LDFLAGS) -L$(CURDIR)/../faac/root/usr/lib -L$(CURDIR)/../opencore-amr/root/usr/lib" \
	--enable-hardcoded-tables --enable-libopencore-amrnb --enable-nonfree \
	--enable-libfaac --enable-decoder=aac --enable-decoder=h264 \
	--enable-decoder=h263 --enable-decoder=mpeg4 --enable-decoder=alac \
	--enable-decoder=qcelp --enable-decoder=mp3 --enable-version3 \
	--enable-decoder=libopencore_amrnb --enable-demuxer=mp3 --enable-demuxer=mov \
	--enable-demuxer=aac --enable-demuxer=amr --enable-demuxer=wav \
	--enable-muxer=mp4 --enable-muxer=tgp --enable-muxer=tg2 --enable-muxer=mov \
	--enable-encoder=h263 --enable-encoder=mpeg4 --enable-encoder=libfaac \
	--enable-encoder=libopencore_amrnb --enable-parser=mpegaudio \
	--disable-postproc --enable-decoder=vorbis --enable-demuxer=ogg \
	--enable-parser=aac --enable-protocol=file \
	--enable-neon

build: $(DIR) $(DIR)/config.mak
	$(MAKE) -C "$(DIR)" all
	mkdir -p root
	$(MAKE) -C "$(DIR)" install DESTDIR="$(CURDIR)/root"

install: build
	cp -rv \
		root/usr/lib/libavcodec.so* \
		root/usr/lib/libavdevice.so* \
		root/usr/lib/libavformat.so* \
		root/usr/lib/libavutil.so* \
		root/usr/lib/libswscale.so* \
			$(DESTDIR)/lib

clean:
	rm -rf "$(DIR)" root

.PHONY: all unpack configure build install clean


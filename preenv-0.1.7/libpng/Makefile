FETCH_URL:=http://prdownloads.sourceforge.net/libpng/libpng-1.2.44.tar.gz
DIR:=libpng-1.2.44

FETCH_FILE:=$(notdir $(FETCH_URL))

all: build

fetch: $(FETCH_FILE)

$(FETCH_FILE):
	wget -O "$@" "$(FETCH_URL)"

unpack: $(DIR)

$(DIR): $(FETCH_FILE)
	tar zxf $<

$(DIR)/config.status:
	cd "$(DIR)" && ./configure --prefix=/usr

build: $(DIR) $(DIR)/config.status
	$(MAKE) -C "$(DIR)" all
	mkdir -p root
	$(MAKE) -C "$(DIR)" install DESTDIR="$(CURDIR)/root"

install: build
	cp -rv root/usr/lib/libpng.so* root/usr/lib/libpng12.so* $(DESTDIR)/lib

clean:
	rm -rf "$(DIR)" root

.PHONY: all unpack configure build install clean


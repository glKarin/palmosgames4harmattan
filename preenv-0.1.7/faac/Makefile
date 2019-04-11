FETCH_URL:=http://palm.cdnetworks.net/opensource/2.0.0/faac-1.24.tar.gz
DIR:=faac

FETCH_FILE:=$(notdir $(FETCH_URL))

all: build

fetch: $(FETCH_FILE)

$(FETCH_FILE):
	wget -O "$@" "$(FETCH_URL)"

unpack: $(DIR)

$(DIR): $(FETCH_FILE)
	tar zxf $<

$(DIR)/config.status:
	cd "$(DIR)" && ./bootstrap
	cd "$(DIR)" && ./configure --prefix=/usr

build: $(DIR) $(DIR)/config.status
	$(MAKE) -C "$(DIR)" all
	mkdir -p root
	$(MAKE) -C "$(DIR)" install DESTDIR="$(CURDIR)/root"

install: build
	cp -rv root/usr/lib/libfaac.so* $(DESTDIR)/lib

clean:
	rm -rf "$(DIR)" root

.PHONY: all unpack configure build install clean


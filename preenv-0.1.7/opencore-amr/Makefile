FETCH_URL:=http://downloads.sourceforge.net/project/opencore-amr/opencore-amr/0.1.2/opencore-amr-0.1.2.tar.gz
DIR:=opencore-amr-0.1.2

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
	cp -rv root/usr/lib/libopencore-amr[nw]b.so* $(DESTDIR)/lib

clean:
	rm -rf "$(DIR)" root

.PHONY: all unpack configure build install clean


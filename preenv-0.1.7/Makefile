SUBDIRS:=libstub libpng faac opencore-amr ffmpeg libsdl-mixer libsdlpre desktopgen

CFLAGS?=-Os -g -Wall
LDFLAGS?=
IFLAGS?=

export CFLAGS
export LDFLAGS
export IFLAGS

LIBSTUBS:=libcjson.so libluna-prefs.so libpdl.so libSDL_cinema.so libgdbus.so \
	liblunaservice.so libPiranha.so libgdbus.so.0 libmjson.so libhid.so \
	libpalmvibe.so libsaxparse.so libPmIpcLib.so libNPAPIInterface.so \
	libdbus-gmain.so.0 libnapp.so

all: $(SUBDIRS)

$(SUBDIRS): %: 
	$(MAKE) -C "$@" all

install: install-before $(addprefix install-,$(SUBDIRS))
	install scripts/env.sh scripts/wrapper.sh $(DESTDIR)/
	cd $(DESTDIR)/lib && for i in $(LIBSTUBS); do \
		ln -sf libstub.so "$$i"; \
	done;
	@echo Everything installed to $(DESTDIR).

install-before:
	@if [ -z "$(DESTDIR)" ]; then \
		echo "DESTDIR is not set! (read INSTALL file!)"; \
		exit 1; \
	fi
	@echo Installing to $(DESTDIR)...
	mkdir -p $(DESTDIR)/lib $(DESTDIR)/bin $(DESTDIR)/data/apps/usr/palm/applications

$(addprefix install-,$(SUBDIRS)): %:
	$(MAKE) -C "$(subst install-,,$@)" install

bindist:
	rm -rf temp-dist
	mkdir -p temp-dist/preenv
	$(MAKE) install DESTDIR=$(CURDIR)/temp-dist/preenv
	cp README temp-dist/preenv
	tar --owner=root -C temp-dist -czvf preenv.tar.gz preenv
	rm -rf temp-dist
	@echo preenv.tar.gz is ready.

clean: $(addprefix clean-,$(SUBDIRS))
	rm -rf temp-dist preenv.tar.gz

$(addprefix clean-,$(SUBDIRS)): %:
	$(MAKE) -C "$(subst clean-,,$@)" clean

.PHONY: all $(SUBDIRS) install install-before bindist clean


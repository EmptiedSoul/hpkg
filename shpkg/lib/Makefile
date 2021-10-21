VERSION = $(shell ./get-version)

all: set-version

install:
	@mkdir -p $(DESTDIR)/usr/lib
	@echo "  INSTALL     hpkglib.sh"
	@install ./hpkglib.sh ${DESTDIR}/usr/lib/hpkglib.sh

set-version:
	@echo "  VERSION     hpkglib.sh.in"
	@sed "s/@VERSION@/$(VERSION)/" hpkglib.sh.in > hpkglib.sh

clean:
	@echo "  CLEAN       hpkglib.sh"
	@rm -f hpkglib.sh

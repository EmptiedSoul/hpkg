all: mk-lib

clean: clean-lib

mk-lib:
	@echo "  MAKE        lib"
	@$(MAKE) -e -C lib

clean-lib:
	@echo "  MAKECLEAN   lib"
	@$(MAKE) -e clean -C lib

install: install-lib install-hpkgs

install-lib:
	@echo "  MAKEINSTALL lib"
	@$(MAKE) -e install -C lib
install-hpkgs:
	@echo "  INSTALL     hpkg"
	@install -m755 hpkg ${DESTDIR}/usr/bin/hpkg
	@echo "  INSTALL     hpkg-explode"
	@install -m755 hpkg-explode ${DESTDIR}/usr/bin/hpkg-explode
	@echo "  INSTALL     hpkg-info"
	@install -m755 hpkg-info ${DESTDIR}/usr/bin/hpkg-info
	@echo "  INSTALL     hpkg-remove"
	@install -m755 hpkg-remove  ${DESTDIR}/usr/bin/hpkg-remove

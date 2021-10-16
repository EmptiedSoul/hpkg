DESTDIR=/
export DESTDIR

MAKEFLAGS+= --no-print-directory

all: make-hardman make-hpkg make-i18n
install: install-hardman install-hpkg install-packing install-i18n
clean: clean-hardman clean-hpkg clean-i18n

make-hardman:
	@echo "  MAKE        hardman"
	@$(MAKE) -e -C hardman
make-hpkg: make-shpkg make-chpkg

make-shpkg: 
	@echo "  MAKE        shpkg"
	@$(MAKE) -e -C shpkg
make-chpkg:
	@echo "  MAKE        chpkg"
	@$(MAKE) -e -C chpkg

make-i18n:
	@echo "  MAKE        i18n"
	@$(MAKE) -e -C i18n

install-hardman:
	@echo "  MAKEINSTALL hardman"
	@$(MAKE) -e install -C hardman

install-hpkg: install-shpkg install-chpkg

install-shpkg:
	@echo "  MAKEINSTALL shpkg"
	@$(MAKE) -e install -C shpkg
install-chpkg:
	@echo "  MAKEINSTALL chpkg"
	@$(MAKE) -e install -C chpkg

install-packing:
	@echo "  MAKEINSTALL packing"
	@$(MAKE) -e install -C packing
install-i18n:
	@echo "  MAKEINSTALL i18n"
	@$(MAKE) -e install -C i18n

clean-hardman:
	@echo "  MAKECLEAN   hardman"
	@$(MAKE) -e clean -C hardman
clean-hpkg: clean-shpkg clean-chpkg

clean-shpkg:
	@echo "  MAKECLEAN   shpkg"
	@$(MAKE) -e clean -C shpkg
clean-chpkg:
	@echo "  MAKECLEAN   chpkg"
	@$(MAKE) -e clean -C chpkg
clean-i18n:
	@echo "  MAKECLEAN   i18n"
	@$(MAKE) -e clean -C i18n



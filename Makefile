DESTDIR=/
export DESTDIR

MAKEFLAGS+= --no-print-directory

all: make-hardman make-hpkg make-i18n
install: install-hardman install-hpkg install-packing install-i18n
clean: clean-hardman clean-hpkg clean-i18n

make-hardman:
	@echo "  MAKE        hardman"
	@$(MAKE) -e -C hardman
make-hpkg:
	@echo "  MAKE        hpkg"
	@$(MAKE) -e -C hpkg
make-i18n:
	@echo "  MAKE        i18n"
	@$(MAKE) -e -C i18n

install-hardman:
	@echo "  MAKEINSTALL hardman"
	@$(MAKE) -e install -C hardman
install-hpkg:
	@echo "  MAKEINSTALL hpkg"
	@$(MAKE) -e install -C hpkg
install-packing:
	@echo "  MAKEINSTALL packing"
	@$(MAKE) -e install -C packing
install-i18n:
	@echo "  MAKEINSTALL i18n"
	@$(MAKE) -e install -C i18n

clean-hardman:
	@echo "  MAKECLEAN   hardman"
	@$(MAKE) -e clean -C hardman
clean-hpkg:
	@echo "  MAKECLEAN   hpkg"
	@$(MAKE) -e clean -C hpkg
clean-i18n:
	@echo "  MAKECLEAN   i18n"
	@$(MAKE) -e clean -C i18n



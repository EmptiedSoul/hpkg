all: resolv 
install: resolv-install install-modules install-hardman install-hparts
clean: resolv-clean

resolv:
	@echo "  MAKE        resolver"
	@$(MAKE) -e -C resolver 
resolv-install:
	@echo "  MAKEINSTALL resolver"
	@$(MAKE) -e install -C resolver
resolv-clean:
	@echo "  MAKECLEAN   resolver"
	@$(MAKE) -e clean -C resolver
install-modules: install-hparts
	@echo "  MAKEINSTALL default-plugins"
	@$(MAKE) -e install -C default-plugins
install-hardman:
	@echo "  INSTALL     hardman"
	@mkdir -p $(DESTDIR)/usr/bin
	@install -m755 hardman $(DESTDIR)/usr/bin/hardman
install-hparts:
	@echo "  INSTALL     hparts"
	@mkdir -p $(DESTDIR)/usr/bin
	@install -m755 hparts $(DESTDIR)/usr/bin/hparts

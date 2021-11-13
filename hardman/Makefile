WITH_DARIA=1

all_deps := resolv
install_deps := resolv-install install-modules install-hardman install-hparts
clean_deps := resolv-clean

ifeq '$(WITH_DARIA)' '1'	
all_deps += make-daria
install_deps += install-daria
clean_deps += clean-daria
endif

all: $(all_deps)
install: $(install_deps)
clean: $(clean_deps)

resolv:
	@echo "  MAKE        resolver"
	@$(MAKE) -e -C resolver 
make-daria:
	@echo "  MAKE        daria"
	@$(MAKE) -e -C daria
install-daria:
	@echo "  MAKEINSTALL daria"
	@$(MAKE) -e install -C daria 
clean-daria:
	@echo "  MAKECLEAN   daria"
	@$(MAKE) -e clean -C daria
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

all: en.mo ru.mo 

install: install-ru.mo install-en.mo

install-ru.mo: ru.mo
	@echo "  INSTALL     ru.mo"
	@install ru.mo ${DESTDIR}/usr/share/locale/ru/LC_MESSAGES/hpkg.mo	
install-en.mo: en.mo
	@echo "  INSTALL     en.mo"
	@install en.mo ${DESTDIR}/usr/share/locale/en/LC_MESSAGES/hpkg.mo	
clean:
	@echo "  CLEAN       i18n"
	@rm -rf index.lokalize lokalize-scripts main.lqa *.mo 
en.mo: translations/en/en.po
	@echo "  MSGFMT      en.mo"
	@msgfmt -v translations/en/en.po -o en.mo
ru.mo: translations/ru/ru.po
	@echo "  MSGFMT      ru.mo"
	@msgfmt -v translations/ru/ru.po -o ru.mo

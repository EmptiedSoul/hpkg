all: en.mo ru.mo 
	@echo "-- Done --"
clean:
	rm -rf index.lokalize lokalize-scripts main.lqa *.mo 
en.mo: translations/en/en.po
	msgfmt -v translations/en/en.po -o en.mo
ru.mo: translations/ru/ru.po
	msgfmt -v translations/ru/ru.po -o ru.mo

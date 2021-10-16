CC = gcc
BINDIR = /bin
PREFIX = /usr
VERSION = $(shell ../shpkg/lib/get-version)
DESTDIR = 
ADDFLAGS =
CFLAGS = -Ofast -pipe -fomit-frame-pointer -flto -fuse-linker-plugin -fuse-ld=gold -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block -I./libhard
DBGFLAGS = -g3 -ggdb3
WARNFLAGS = -Wall -Wextra -Wshadow -Werror -Wno-unused-result
LDFLAGS = -Wl,-O1  -lhrd -lhard -L./libhard 

MAKEFLAGS += --no-print-directory

FLAGS := ${CFLAGS} ${DBGFLAGS} ${WARNFLAGS} ${ADDFLAGS} -DVERSION=\"${VERSION}\"

all: make-libhard chpkg-info #chpkg chpkg-info 

make-libhard:
	@echo "  MAKE        libhard"
	@$(MAKE) -e -C libhard
libhard-install: libhard
	@echo "  MKINST      $@"
	@$(MAKE) -e -C libhard install

chpkg: chpkg.o
	@echo "  CCLD        $@"
	@$(CC) -o $@ $^ ${FLAGS} ${LDFLAGS} 
chpkg-info: chpkg-info.o
	@echo "  CCLD        $@"
	@$(CC) -o $@ $^ ${FLAGS} ${LDFLAGS} 

chpkg.o: chpkg.c
	@echo "  CC          $@"
	@$(CC) -o $@ -c $< ${FLAGS} -DPROGNAME=\"hpkg\"
chpkg-info.o: chpkg-info.c
	@echo "  CC          $@"
	@$(CC) -o $@ -c $< ${FLAGS} -DPROGNAME=\"hpkg-info\"

clean: libhard-clean
	@echo "  CLEAN       chpkg"
	@rm -f *.o chpkg chpkg-info
libhard-clean:
	@echo "  MAKECLEAN   libhard"
	@$(MAKE) -e clean -C    libhard

install: all libhard-install
	#@echo "  INSTALL chpkg"
	#@install -m755 chpkg ${DESTDIR}/${PREFIX}/${BINDIR}/chpkg
	@echo "  INSTALL      chpkg-info"
	@install -m755 chpkg-info ${DESTDIR}/${PREFIX}/${BINDIR}/chpkg-info

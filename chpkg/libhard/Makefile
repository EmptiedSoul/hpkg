CC = gcc
ADDFLAGS =
CFLAGS = -Ofast -pipe -fomit-frame-pointer -flto -fuse-linker-plugin -fuse-ld=gold -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block
DBGFLAGS = -g3 -ggdb3
WARNFLAGS = -Wall -Wextra -Wshadow -Werror -Wno-unused-result -Wno-unused-command-line-argument
LDFLAGS = -Wl,-O1  -lhrd

MAKEFLAGS += --no-print-directory

FLAGS := ${CFLAGS} ${DBGFLAGS} ${WARNFLAGS} ${ADDFLAGS} -fPIC 

objects = $(patsubst %.c,%.o,$(wildcard *.c))

all: libhard.so

libhard.so: ${objects}
	@echo "  CCLD        $@"
	@$(CC) -o $@ $^ -shared ${FLAGS} ${LDFLAGS}

%.o: %.c
	@echo "  CC          $@"
	@$(CC) -o $@ -c $< $(FLAGS)

clean:
	@echo "  CLEAN       libhard"
	@rm -f ${objects} libhard.so
install: all
	@echo "  INSTALL     libhard.so"
	@install -m755 libhard.so ${DESTDIR}/usr/lib/libhard.so
	@echo "  INSTALL     libhard.h" 
	@install -m644 libhard.h  ${DESTDIR}/usr/include/libhard.h
	@echo "  INSTALL     libhard.pc"
	@install -m644 libhard.pc ${DESTDIR}/usr/lib/pkgconfig/libhard.pc

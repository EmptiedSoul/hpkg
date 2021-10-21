DESTDIR=/
CC=gcc
ADDFLAGS=
CFLAGS=	-Ofast -pipe -flto -fuse-linker-plugin -fuse-ld=gold
WARNFLAGS= -Wall -Werror -Wextra -Wshadow -Wno-unused-result -Wno-unused-command-line-argument
DBGFLAGS= -g3 -ggdb3
LDFLAGS= -Wl,-O1 -lhrd

FLAGS = $(CFLAGS) $(WARNFLAGS) $(DBGFLAGS) $(LDFLAGS) $(ADDFLAGS) -fPIC

all: libhpm.so

objects = $(patsubst %.c,%.o,$(wildcard *.c))

libhpm.so: ${objects}
	@echo "  CCLD        $@"
	@$(CC) -o $@ $^ -shared $(FLAGS)
%.o: %.c
	@echo "  CC          $@"
	@$(CC) -c $< $(FLAGS) 
clean:
	@echo "  CLEAN       libhpm"
	@rm -f *.o libhpm.so
install: libhpm.so libhpm.h
	@echo "  INSTALL     libhpm"
	@mkdir -p $(DESTDIR)/usr/lib/pkgconfig $(DESTDIR)/usr/include
	@install -m755 ./libhpm.so $(DESTDIR)/usr/lib/
	@install -m644 ./libhpm.h  $(DESTDIR)/usr/include/
	@install -m644 ./libhpm.pc $(DESTDIR)/usr/lib/pkgconfig/

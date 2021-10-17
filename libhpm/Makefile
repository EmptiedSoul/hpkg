all: libhpm.so

objects = $(patsubst %.c,%.o,$(wildcard *.c))

libhpm.so: ${objects}
	@echo "  CCLD        $@"
	@gcc -o $@ $^ -shared -Wl,-O1 -O2 -Wall -Werror -Wextra -Wshadow -flto -fuse-linker-plugin -fuse-ld=gold -pipe -g2 -ggdb -lhrd
%.o: %.c
	@echo "  CC          $@"
	@gcc -c $< -O2 -Wall -Werror -Wextra -Wno-unused-result -Wshadow -flto -fuse-linker-plugin -fuse-ld=gold -pipe -fPIC -g2 -ggdb 
clean:
	@echo "  CLEAN       libhpm"
	@rm -f *.o libhpm.so
install: libhpm.so libhpm.h
	@echo "  INSTALL     libhpm"
	@install -m755 ./libhpm.so /usr/lib/
	@install -m644 ./libhpm.h  /usr/include/

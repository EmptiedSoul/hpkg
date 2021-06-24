CC = gcc
TEST_PKG = gcc
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS =
RCFLAGS := $(CFLAGS) -Ofast -flto -fuse-linker-plugin -fuse-ld=gold
#DBGFLAGS = -g2 -ggdb
DBGFLAGS =
WARNFLAGS = -Wall -Wextra -pedantic -Wshadow -Wno-unused-result -Werror

all: hsfd-resolv

hsfd-resolv: ${OBJS}
	${CC} -o hsfd-resolv ${OBJS} ${RCFLAGS} ${WARNFLAGS} ${DBGFLAGS}

%.o: %.c
	${CC} -o $@ -c $< $(RCFLAGS) ${WARNFLAGS} ${DBGFLAGS}
clean:
	rm -f *.o hsfd-resolv
test: all
	./hsfd-resolv ${TEST_PKG}

pgo: clean
	make CFLAGS="${CFLAGS} -fprofile-generate"
	make test TEST_PKG="${TEST_PKG}"
	make clean
	make CFLAGS="${CFLAGS} -fprofile-use"
	rm *.gcda

install: all
	mkdir -p /usr/libexec/hardman
	install -m755 ./hsfd-resolv /usr/libexec/hardman/hsfd-resolv


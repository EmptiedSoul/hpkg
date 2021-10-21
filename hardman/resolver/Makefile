CC = gcc
TEST_PKG = gcc
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
CFLAGS = -Ofast -flto -fuse-linker-plugin -fuse-ld=gold
ADDFLAGS = 
RCFLAGS := $(CFLAGS) $(ADDFLAGS) 
#DBGFLAGS = -g2 -ggdb
DBGFLAGS =
WARNFLAGS = -Wall -Wextra -pedantic -Wshadow -Wno-unused-result -Werror -Wno-unused-command-line-argument

all: hsfd-resolv

hsfd-resolv: ${OBJS}
	@echo "  LINK        hsfd-resolv"
	@${CC} -o hsfd-resolv ${OBJS} ${RCFLAGS} ${WARNFLAGS} ${DBGFLAGS}

%.o: %.c
	@echo "  CC          $@"
	@${CC} -o $@ -c $< $(RCFLAGS) ${WARNFLAGS} ${DBGFLAGS}
clean:
	@echo "  CLEAN       hsfd-resolv"
	@rm -f *.o hsfd-resolv
test: all
	@echo "  TEST        hsfd-resolv"
	@./hsfd-resolv ${TEST_PKG}

pgo: clean
	@make CFLAGS="${CFLAGS} -fprofile-generate"
	@make test TEST_PKG="${TEST_PKG}"
	@make clean
	@make CFLAGS="${CFLAGS} -fprofile-use"
	@rm *.gcda

install: all
	@mkdir -p $(DESTDIR)/usr/libexec/hardman
	@echo "  INSTALL     hsfd-resolv"
	@mkdir -p ${DESTDIR}/usr/libexec/hardman
	@install -m755 ./hsfd-resolv ${DESTDIR}/usr/libexec/hardman/hsfd-resolv


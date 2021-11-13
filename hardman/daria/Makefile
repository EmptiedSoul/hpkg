CC = gcc
CXX = g++
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))
OBJS += $(patsubst %.cpp,%.o,$(wildcard *.cpp))
CFLAGS = -Ofast -flto -fuse-linker-plugin -fuse-ld=gold
ADDFLAGS = 
RCFLAGS := $(CFLAGS) $(ADDFLAGS) 
DBGFLAGS = -g3 -ggdb
#DBGFLAGS =
WARNFLAGS = -Wall -Werror -Wno-switch -Wno-write-strings -Wextra -Wshadow -Wno-unused-result -Wno-unused-parameter -Wno-unused-command-line-argument

all: daria

daria: ${OBJS}
	@echo "  CXXLD       daria"
	@${CXX} -o daria ${OBJS} ${RCFLAGS} ${WARNFLAGS} ${DBGFLAGS} -laria2 -lhrd

%.o: %.c
	@echo "  CC          $@"
	@${CC} -o $@ -c $< $(RCFLAGS) ${WARNFLAGS} ${DBGFLAGS}
%.o: %.cpp
	@echo "  CXX         $@"
	@${CXX} -o $@ -c $< $(RCFLAGS) ${WARNFLAGS} ${DGBFLAGS}
clean:
	@echo "  CLEAN       daria"
	@rm -f *.o daria
install: all
	@mkdir -p $(DESTDIR)/usr/libexec/hardman
	@echo "  INSTALL     daria"
	@mkdir -p ${DESTDIR}/usr/libexec/hardman
	@install -m755 ./daria ${DESTDIR}/usr/libexec/hardman/daria


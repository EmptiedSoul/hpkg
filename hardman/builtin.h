#ifndef HARDMAN_BUILTIN_H
#define HARDMAN_BUILTIN_H

#include <stdarg.h>

extern void init_hardman_io();
extern void verbose_msg(char* fmt, ...);
extern void msg(char* fmt, ...);
extern void warn(char* fmt, ...);
extern void error(int code);

#endif

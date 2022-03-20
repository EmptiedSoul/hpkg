/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been received a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include <libintl.h>

#include <libhpm.h>

#include "variables.h"

static bool no_prefix = false;

static char* def_msg_prefix
	= "\e[34;1m==>\e[0m ";
static char* def_msg_prefix_nc 
	= "==> ";
static char* def_warn_prefix
	= "\e[31;1m<!>\e[0m ";
static char* def_warn_prefix_nc
	= "<!> ";

static char* msg_prefix; 
static char* warn_prefix;

void init_hardman_io() {
	bool  no_color;
	
	char* custom_msg_prefix = NULL;
	char* custom_warn_prefix = NULL;

	no_prefix = getenv("H_NO_PREFIX") || hrd_cfg_get_string(hardman_config, 
			"General", "no_prefix");
	no_color  = getenv("H_NO_COLOR")  || hrd_cfg_get_string(hardman_config,
			"General", "no_color");
	custom_msg_prefix = getenv("H_MSG_PREFIX") ? getenv("H_MSG_PREFIX") :
						hrd_cfg_get_string(hardman_config,
								"General", "msg_prefix");
	custom_warn_prefix = getenv("H_WARN_PREFIX") ? getenv("H_WARN_PREFIX") :
						hrd_cfg_get_string(hardman_config,
								"General", "warn_prefix");
	
	if (no_color) {
		msg_prefix  = def_msg_prefix_nc;
		warn_prefix = def_warn_prefix_nc;
	} else {
		msg_prefix  = def_msg_prefix;
		warn_prefix = def_warn_prefix;
	}

	if (custom_msg_prefix)
		msg_prefix = custom_msg_prefix;
	if (custom_warn_prefix)
		warn_prefix = custom_warn_prefix;
} 

void verbose_msg(char* fmt, ...) {
	va_list args;
	if (verbose_lvl) {
		va_start(args, fmt);
		vprintf(fmt, args);
	}
}

void msg(char* fmt, ...) {
	va_list args;
	if (!silent_lvl) {
		va_start(args, fmt);
		if (no_prefix) {
			vprintf(fmt, args);
		} else {
			char* string = NULL;
			vasprintf(&string, fmt, args);
			printf("%s%s", msg_prefix, string);
			free(string);
		}
	}
}

void warn(char* fmt, ...) {
	va_list args;
	if (!silent_lvl) {
		va_start(args, fmt);
		if (no_prefix) {
			vfprintf(stderr, fmt, args);
		} else {
			char* string = NULL;
			vasprintf(&string, fmt, args);
			fprintf(stderr, "%s%s", warn_prefix, string);
			free(string);
		}
	}
}

void error(int code) {
	warn("(%d) %s\n", code,
			gettext("Failing due to previous warnings"));
	if (verbose_lvl > 1)
		perror(gettext("Last set errno"));
	exit(code);
}

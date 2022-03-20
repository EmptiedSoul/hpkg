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
#include <sys/stat.h>
#include <dirent.h>
#include <libintl.h>

#include <libhpm.h>

#include "builtin.h"
#include "variables.h"

#define max(x,y) (x < y) ? y : x

static inline void print_help();
static inline void populate_commands();
static inline void register_builtins();
static inline void print_version();
static inline void print_command_help(char* cmd);
static inline void print_command_version(char* cmd);

hrd_hashmap* hardman_mod_help;
hrd_hashmap* hardman_mod_vers;
hrd_config*  hardman_config;
hrd_hashmap* hardman_commands;
int verbose_lvl;
int silent_lvl;
batch_mode_t batch_mode;

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("hpkg", "/usr/share/locale");
	textdomain("hpkg");

	int opt;
	int long_optind;
	static struct option long_opts[] = {
		{"help",		optional_argument,	0, 'h'},
		{"usage",		optional_argument,	0, 'h'},
		{"version",		optional_argument,	0, 'V'},
		{"verbose",		optional_argument,	0, 'v'},
		{"batch",		no_argument,		0, 'b'},
		{"yes",			no_argument,		0, 'y'},
		{"no",			no_argument,		0, 'n'},
		{0,			0,			0,  0 },
	};

	hardman_config = hrd_cfg_read_at("/etc/hardman.conf");
	
	if (!hardman_config) {
		warn(gettext("Can not read /etc/hardman.conf. Is it present?\n"));
		error(32);
	}

	init_hardman_io();

	if (argc < 2)
		print_help();
	
	int last_option;
	bool is_long_unrecognized;

	while ((opt = getopt_long(argc, argv, "-:V::vh::bny", long_opts, &long_optind)) != -1) {
		bool nuff_parsing = false;
		switch (opt) {
			case 'V':
				if (optarg)
					print_command_version(optarg);
				else 
					print_version();
				break;
			case 'h':
				if (optarg)
					print_command_help(optarg);
				else
					print_help();
				break;
			case 'v':
				if (optarg)
					verbose_lvl = atoi(optarg);
				else 
					verbose_lvl++;
				break;
			case 'b':
				batch_mode = BATCH_DEF;
				break;
			case 'n':
				batch_mode = BATCH_NO;
				break;
			case 'y':
				batch_mode = BATCH_YES;
				break;
			default:
				switch ((char)optopt) {
				case '\0':
					if (*argv[optind-1] == '-') {
						warn(gettext("Unknown option \"%s\". Consult a manpage or '--help' option\n"),
								argv[optind-1]);
						error(32);
					} else {
						last_option = optind - 1;
						nuff_parsing = true;
					}
					break;
				default:
					if (*argv[optind-1] == '-') {
						warn(gettext("Unknown option '-%c' in \"%s\". Consult a manpage or '--help' option\n"), 
								(char)optopt,
								argv[optind-1]);
						error(32);
					} else {
						last_option = optind - 1;
						nuff_parsing = true;
					}
					break;
				}
		}
		if (nuff_parsing)
			break;
	}

	if (argv[0] == argv[last_option])
		return 0;
	populate_commands();

	char** mod_argv = argv + last_option;
	int    mod_argc = argc - last_option;

	int (*mod_main)(int, char**) = hrd_hashmap_get_value(hardman_commands, argv[last_option]);

	if (!mod_main) {
		warn("%s: no such command\n", argv[last_option]);
		error(1);
	}

	register_builtins();

	return mod_main(mod_argc, mod_argv);
}




int max_option_len = 0;
static void set_option_size(char* option) {
	int size = strlen(option) + 7;
	max_option_len = max(max_option_len, size);
}

static void put_option(char* option, char* description) {
	/* Warning doesnt make sense */
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wformat"
	printf("  %s%*.*c%s\n", option,
			(int) (max_option_len - (strlen(option) + 3)),
			(int) (max_option_len - (strlen(option) + 3)),
			' ', description);
	#pragma GCC diagnostic pop
}

static inline void populate_commands() {
	DIR* dir = opendir("/usr/lib/hardman/modules/enabled/");

	if (!dir) {
		warn("%s\n", gettext("Unable to read hardman module dir, is installation broken?"));
		error(32);
	}

	if (!hardman_commands)
		hardman_commands = hrd_hashmap_create(30);
	if (!hardman_mod_help)
		hardman_mod_help = hrd_hashmap_create(30);
	if (!hardman_mod_vers)
		hardman_mod_vers = hrd_hashmap_create(30);

	for (struct dirent* file = readdir(dir); file; file = readdir(dir)) {
		if (*file->d_name == '.') continue;
		file->d_name[strlen(file->d_name) - 3] = '\0';
		hpm_module* mod = hpm_load_module(file->d_name, HPM_MODULE_LOAD);
		void (*populate_hardman_commands)(hrd_hashmap*);
		void (*populate_hardman_help)(hrd_hashmap*);
		void (*populate_hardman_vers)(hrd_hashmap*);
		populate_hardman_commands = hpm_get_module_function(mod->name,
				"populate_hardman_commands");
		populate_hardman_help = hpm_get_module_function(mod->name,
				"populate_hardman_help");
		populate_hardman_vers = hpm_get_module_function(mod->name,
				"populate_hardman_version");
		if (populate_hardman_commands)
			populate_hardman_commands(hardman_commands);
		if (populate_hardman_help)
			populate_hardman_help(hardman_mod_help);
		if (populate_hardman_vers)
			populate_hardman_vers(hardman_mod_vers);

	}
	closedir(dir);
}

static inline void print_help() {
	puts(gettext("hardman - high-level package managing utility"));
	puts(	     "");
	puts(gettext("hardman [hardman options] <command> [command options]"));
	puts(        "");
	puts(gettext("hardman options:"));

	set_option_size("-h, --help, --usage[=<command>]");
	set_option_size("-V, --version[=<command>]");
	set_option_size("-v, --verbose");
	set_option_size("-b, --batch");
	set_option_size("-y, --yes");
	set_option_size("-n, --no");

	put_option("-h, --help, --usage[=<command>]", 
			gettext("Print help message [for <command>]"));
	put_option("-V, --version[=<command>]", 
			gettext("Print version and copyright information [for <command>]"));
	put_option("-v, --verbose", gettext("Be verbose"));
	put_option("-b, --batch", gettext("Do not ask questions, use default answers"));
	put_option("-y, --yes",	gettext("Assume user's answer is always \"yes\""));
	put_option("-n, --no",  gettext("Assume user's answer is always \"no\""));

	printf("\n%s:\n", gettext("Available commands"));

	populate_commands();

	for (hrd_hashmap_slot* slot = hardman_commands->first; slot; slot = slot->next) {
		printf("  %s\n", slot->key);
	}

	printf("\n%s\n", gettext("Use --help=<command> for command options"));
	
	exit(0);
}

static inline void register_builtins() {
	hpm_module* mod = malloc(sizeof(hpm_module));
	hrd_hashmap* funcs = hrd_hashmap_create(20);

	hrd_hashmap_set_value(funcs, "msg", msg);
	hrd_hashmap_set_value(funcs, "verbose_msg", verbose_msg);
	hrd_hashmap_set_value(funcs, "warn", warn);
	hrd_hashmap_set_value(funcs, "error", error);

	mod->functions = funcs;

	hrd_hashmap_set_value(hpm_loaded_modules, "builtin", mod);
}

static inline void print_command_help(char* command) {
	populate_commands();
	void (*print_cmd_help)(void) = hrd_hashmap_get_value(hardman_mod_help, command);
	if (print_cmd_help)
		print_cmd_help();
	else {
		if (hrd_hashmap_get_value(hardman_commands, command)) {
			warn(gettext("%s: no help information provided by developer\n"), command);
			error(1);
		} else {
			warn(gettext("%s: no such command\n"), command);
			error(1);
		}
	}
}

static inline void print_version() {
	/* From chpkg/hpkglib.h */
	printf( "%s %s %s hpkg-utils (%s, %s %s)\n" 
		"\n" 
		"Copyright (C) 2020-2022, %s %s hardclanz.org\n" 
		"%s\n" 
		"%s\n" 
		"%s\n", 
		gettext("This is"), 
		PROGNAME, 
		gettext("from"), 
		gettext("C implementation"), 
		gettext("version"), 
		VERSION, 
		gettext("Arseniy 'EmptiedSoul' Lesin"), 
		gettext("from"), 
		gettext("License: GPLv3 or newer: <https://gnu.org/licenses>"), 
		gettext("This is free software: you can freely run, inspect, modify and distribute this program"), 
		gettext("NO WARRANTY AT ALL")); 
	exit(0); 
}

static inline void print_command_version(char* command) {
	populate_commands();
	void (*print_cmd_vers)(void) = hrd_hashmap_get_value(hardman_mod_vers, command);
	if (print_cmd_vers)
		print_cmd_vers();
	else {
		if (hrd_hashmap_get_value(hardman_commands, command)) {
			warn(gettext("%s: no version information provided by developer\n"), command);
			error(1);
		} else {
			warn(gettext("%s: no such command\n"), command);
			error(1);
		}
	}
}

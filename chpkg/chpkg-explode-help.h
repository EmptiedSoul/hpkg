/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

inline static void print_help() {
	printf("%s\n"
	       "\n"
	       "%s\n"
	       "\n"
	       "%s:\n"
	       "  -V | --version           %s\n"
	       "  -h | --help | --usage    %s\n"
	       "  -v | --verbose           %s\n"
	       "  -s | --silent            %s\n"
	       "  -r | --recursive         %s\n"
	       "\n"
	       "  %s\n"
	       "  %s\n"
	       "                           %s\n"
	       "  %s\n"
	       "  %s\n"
	       "                           %s\n"
	       "\n"
	       "%s\n"
	       "%s\n",

	       gettext("hpkg-explode - extract .hard package"),
	       gettext("hpkg-explode [options] -p <package>"),
	       gettext("Options"),
	       gettext("Print version info and exit"),
	       gettext("Print this help"),
	       gettext("Be verbose"),
	       gettext("Be silent"),
	       gettext("Also extract PAYLOAD.tar.zst"),
	       gettext("-C <directory>"),
	       gettext("--directory=<directory>"),
	       gettext("Extract to <directory>"),
	       gettext("-p <package>"),
	       gettext("--package=<package>"),
	       gettext("Specify package to extract"),
	       gettext("More information available at hpkg-explode(1) manpage"),
	       gettext("Copyright and licensing information can be obtained via '--version' option")
	       );
	exit(1);
}

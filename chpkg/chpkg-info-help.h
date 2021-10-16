#define print_help() { \
	printf("%s\n" \
		"\n" \
		"%s\n" \
		"\n" \
		"%s:\n" \
		"  -V | --version	%s\n" \
		"  -h | --help | --usage %s\n" \
		"  -d			%s\n" \
		"  -a			%s\n" \
		"  %s		%s\n" \
		"  %s		%s\n" \
		"\n" \
		"%s\n" \
		"%s\n", \
		\
		gettext("hpkg-info - obtain information about .hard package"), \
		gettext("hpkg-info [options] -p <package>"), \
		gettext("Options"), \
		gettext("Print version info and exit"), \
		gettext("Print this help"), \
		gettext("Dump entire METADATA file"), \
		gettext("Print as \"KEY=VALUE\""), \
		gettext("-k <key>"), \
		gettext("Specify key to obtain"), \
		gettext("-o <file>"), \
		gettext("Specify output (default stdout)"), \
		gettext("More information available at hpkg-info(1) manpage"), \
		gettext("Copyright and licensing information can be obtained via '--version' option")); \
	exit(1); }

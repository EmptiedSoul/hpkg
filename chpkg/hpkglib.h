/* pointer("Doing smth") -> "==> Doing smth" */
#define pointer(fmt, ...) \
{ \
	if (!silent) { \
		if (!no_color){ \
			printf("\e[34;1m==>\e[0m ");	\
			printf(fmt, ##__VA_ARGS__);\
			puts(""); \
		} else { \
			printf("==> "); \
			printf(fmt, ##__VA_ARGS__); \
			puts(""); \
		} \
	} \
}

/* warn("Warning...") -> "<!> Warning..." */
#define warn(fmt, ...) \
{ \
	if (!silent) { \
		if (!no_color) { \
			fprintf(stderr, "\e[31;1m<!>\e[0m "); \
			fprintf(stderr, fmt, ##__VA_ARGS__); \
			fputs("\n", stderr); \
		} else { \
			fprintf(stderr, "<!> "); \
			fprintf(stderr, fmt, ##__VA_ARGS__); \
			fputs("\n", stderr); \
		} \
	} \
}

/* error(code) -> "<!> (code) Failing due to previous errors" */
#define error(code) \
{ \
	warn("(%d) %s", code, gettext("Failing due to previous warnings")); \
	exit(32); \
}

#define print_version() \
{ \
	printf( "%s %s %s hpkg-utils (%s, %s %s)\n" \
		"\n" \
		"Copyright (C) 2020-2021, %s %s hardclanz.org\n" \
		"%s\n" \
		"%s\n" \
		"%s\n", \
		gettext("This is"), \
		PROGNAME, \
		gettext("from"), \
		gettext("C implementation"), \
		gettext("version"), \
		VERSION, \
		gettext("Arseniy 'EmptiedSoul' Lesin"), \
		gettext("from"), \
		gettext("License: GPLv3 or newer: <https://gnu.org/license>"), \
		gettext("This is free software: you can freely run, inspect, modify and distribute this program"), \
		gettext("NO WARRANTY AT ALL")); \
	exit(0); \
}

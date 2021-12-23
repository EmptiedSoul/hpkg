/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <locale.h>
#include <string.h>
#include <errno.h>
#include <sys/sendfile.h>
#include <libintl.h>

#include <libhard.h>
#include <libhrd.h>

#include "chpkg-info-help.h"
#include "hpkglib.h"

#define TMP_FILE_TEMPLATE "/tmp/chpkg-info-metadata-XXXXXX"
#define TMP_FILENAME_SIZE sizeof(TMP_FILE_TEMPLATE)+1 /* +1 is for terminating '\0' */

#define TMP_DEC_FILE_TEMPLATE "/tmp/chpkg-info-decrypted-XXXXXX"
#define TMP_DEC_FILENAME_SIZE sizeof(TMP_DEC_FILE_TEMPLATE)+1

int main(int argc, char** argv){
	setlocale(LC_ALL, getenv("LC_ALL"));
	bindtextdomain("hpkg", "/usr/share/locale");
	textdomain("hpkg");

	FILE* out;

	int opt;

	bool no_color = false;
	bool silent = false;
	bool full_dump = false;
	bool print_formatted = false;

	char* key = NULL;

	char* decrypted = NULL;

	char* output = NULL;
	char* pkgname = NULL;

	if (argc < 2)
		print_help();
	if (strcmp(argv[1], "--help") == 0)
		print_help();
	if (strcmp(argv[1], "--usage") == 0)
		print_help();
	if (strcmp(argv[1], "--version") == 0)
		print_version();

	while ((opt = getopt(argc, argv, ":Vhdak:o:p:")) != -1){
		switch(opt){
			case 'V':
				print_version();
				break;
			case 'h':
				print_help();
				break;
			case 'd': 
				full_dump = true;
				break;
			case 'a': 
				print_formatted = true;
				break;
			case 'k':
				key = optarg;
				break;
			case 'o': 
				output = optarg;
				break;
			case 'p':
				pkgname = optarg;
				break;
			case ':':
			default:
				switch(optopt){
					case 'k':
						warn("%s '-k' %s %s. %s", 
								gettext("Option"),
								gettext("requires"),
								gettext("<key> argument"),
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
					case 'o':
						warn("%s '-o' %s %s. %s",
								gettext("Option"),
								gettext("requires"),
								gettext("<file> argument"),
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
					case 'p':
						warn("%s '-p' %s %s. %s",
								gettext("Option"),
								gettext("requires"),
								gettext("<package> argument"),
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
					default:
						warn("%s '-%c'. %s",
								gettext("Unknown option"),
								(char)optopt,
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
				}
		}
	}

	if (pkgname == NULL) {
		warn("%s. %s", gettext("Package is not set"), gettext("Consult a manpage or '--help' option"));
		error(32);
	}

	if (!full_dump && key == NULL) {
		warn("%s", gettext("You should specify a key to obtain"));
		error(32);
	}

	if (!hard_is_hard(pkgname)){
		if (errno == ENOENT) {
			warn("%s: %s", pkgname, strerror(errno));
			error(1);
		} else { /* errno == EINVAL */
			warn("%s: %s", pkgname, gettext("is not a .hard package"));
			error(1);
		}
	}
	
	char* key_comma = strchr(key, ',');
	char** key_list = NULL;
	bool  key_is_list = false;

	if (key_comma) {
		key_is_list = true;
		key_list = hrd_string_split(key, ",");
	}

	char* template = malloc(TMP_FILENAME_SIZE);
	strcpy(template, TMP_FILE_TEMPLATE);

	int metadata_fd = mkstemp(template);
	FILE* metadata = fdopen(metadata_fd, "w+");

	FILE* package = fopen(pkgname, "r");

	if (hard_is_package_encrypted(package)) {
		decrypted = malloc(TMP_DEC_FILENAME_SIZE);
		strcpy(decrypted, TMP_DEC_FILE_TEMPLATE);

		int decrypted_fd = mkstemp(decrypted);
		FILE* decrypted_pkg = fdopen(decrypted_fd, "w+");
		
		hard_decrypt_package(pkgname, decrypted);
		
		fclose(package);

		package = decrypted_pkg;
	}

	hard_split_pkg(package, metadata, NULL);
	
	fflush(package);
	fflush(metadata);

	if (output == NULL) {
		out = stdout;
	} else {
		out = fopen(output, "a");
	}

	if (full_dump) {
		char buffer[1024] = "";
	
		rewind(metadata);

		while (!feof(metadata)) {
			fread(buffer, 1024, 1, metadata);
			fwrite(buffer, 1024, 1, out);
		} 

		fflush(out);

		fclose(out);
		unlink(template);
		free(template);
		fclose(package);
		fclose(metadata);
		if (decrypted != NULL) {
			unlink(decrypted);
			free(decrypted);
		}
		exit(0);
	}
	char* obtained_key = NULL;
	hrd_string_pair* key_vals = NULL;
	if (!key_is_list) {	
		obtained_key = hrd_cfg_get_string_at(template, key);
	} else {
		hrd_string_array_foreach (i, key_list) {
			key_vals = realloc(key_vals, sizeof(hrd_string_pair)*(i+1));
			key_vals[i].key = strdup(key_list[i]);
			key_vals[i].value = NULL;
		}
		hrd_cfg_get_strings_at(template, &key_vals);
	} 

	if (print_formatted) {
		if (!key_is_list)
			fprintf(out, "%s=%s\n", key, obtained_key);
		else {
			hrd_string_pair_array_foreach (i, key_vals) {
				fprintf(out, "%s=%s\n", key_vals[i].key, key_vals[i].value);
			}
		}
	} else {
		if (!key_is_list) {
			if (obtained_key != NULL) {
				hrd_string_discard_chars(obtained_key, '"');
				hrd_string_discard_chars(obtained_key, '(');
				hrd_string_discard_chars(obtained_key, ')');
			}
			fprintf(out, "%s\n", obtained_key);
		} else {
			hrd_string_pair_array_foreach (i, key_vals) {
				if (key_vals[i].value != NULL) {
					hrd_string_discard_chars(key_vals[i].value, '"');
					hrd_string_discard_chars(key_vals[i].value, '(');
					hrd_string_discard_chars(key_vals[i].value, ')');
				}
				fprintf(out, "%s\n", key_vals[i].value);
			}	
		}
	}
	fclose(out);
	unlink(template);
	free(template);
	fclose(package);
	fclose(metadata);
	if (decrypted != NULL) {
		unlink(decrypted);
		free(decrypted);
	}
	if (key_is_list) {
		hrd_string_pair_array_free(key_vals);
		hrd_string_array_free((void**)key_list);
	}
	exit(0);
}

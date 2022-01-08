/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
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
#include <sys/stat.h>
#include <getopt.h>

#include <libhard.h>
#include <libhrd.h>

#include <archive.h>
#include <archive_entry.h>

#include "chpkg-explode-help.h"
#include "hpkglib.h"

#define TMP_FILE_TEMPLATE "/tmp/chpkg-explode-payload-XXXXXX"
#define TMP_FILENAME_SIZE sizeof(TMP_FILE_TEMPLATE)+1 /* +1 is for terminating '\0' */

#define TMP_DEC_FILE_TEMPLATE "/tmp/chpkg-explode-decrypted-XXXXXX"
#define TMP_DEC_FILENAME_SIZE sizeof(TMP_DEC_FILE_TEMPLATE)+1

bool	no_color = false;
bool	silent   = false;

static int copy_data(struct archive *ar, struct archive *aw);

int main(int argc, char** argv) {
	setlocale(LC_ALL, "");
	bindtextdomain("hpkg", "/usr/share/locale");
	textdomain("hpkg");

	int opt;

	bool	verbose  = false;
	bool	recursive= false;
	char*	directory= NULL ;
	char*	package	 = NULL ;
	char*	decrypted= NULL ;

	int long_optind = 0;

	static struct option long_opts[] = {
		{"help",	no_argument,		0, 'h'},
		{"usage",	no_argument,		0, 'h'},
		{"version",	no_argument,		0, 'V'},
		{"verbose",	no_argument,		0, 'v'},
		{"silent",	no_argument,		0, 's'},
		{"recursive",	no_argument,		0, 'r'},
		{"directory",	required_argument,	0, 'C'},
		{"package",	required_argument,	0, 'p'},
		{0,		0,			0,  0 }
	};

	if (argc < 2)
		print_help();

	while ((opt = getopt_long(argc, argv, ":VhvsrRC:p:", long_opts, &long_optind)) != -1) {
		switch (opt){
			case 'V':
				print_version();
				break;
			case 'h':
				print_help();
				break;
			case 'v':
				silent = false;
				verbose = true;
				break;
			case 's':
				silent = true;
				verbose = false;
				break;
			case 'r':
			case 'R':
				recursive = true;
				break;
			case 'C':
				directory = optarg;
				break;
			case 'p':
				package = optarg;
				break;
			case ':':
			default:
				switch((char)optopt) {
					case 'C':
						warn("%s \"%s\" %s %s. %s", 
								gettext("Option"),
								argv[optind-1],
								gettext("requires"),
								gettext("<directory> argument"), 
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
					case 'p':
						warn("%s \"%s\" %s %s. %s", 
								gettext("Option"),
								argv[optind-1],
								gettext("requires"),
								gettext("<package> argument"), 
								gettext("Consult a manpage or '--help' option"));
						error(32);
						break;
					case '\0':
						warn("%s \"%s\". %s", 
								gettext("Unknown option"),
								argv[optind-1],
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
	if (package == NULL) {
		warn("%s. %s", gettext("Package is not set"), gettext("Consult a manpage or '--help' option"));
		error(32);
	}

	if (!hard_is_hard(package)) {
		if (errno == ENOENT) {
			warn("%s: %s", package, strerror(errno));
			error(1);
		} else {
			warn("%s: %s", package, gettext("is not a .hard package"));
			error(1);
		}
	}
	verbose_msg(gettext("%s is a valid .hard package"), package);

	if (directory == NULL) {
		directory = strdup(package);
		directory[strlen(package) - 5] = '\0';
	}
	verbose_msg(gettext("Directory: %s"), directory);

	FILE* pkg = fopen(package, "r");

	if (hard_is_package_encrypted(pkg)) {
		verbose_msg(gettext("%s is encrypted, decrypting..."), package);
		decrypted = malloc(TMP_DEC_FILENAME_SIZE);
		strcpy(decrypted, TMP_DEC_FILE_TEMPLATE);

		int decrypted_fd = mkstemp(decrypted);
		FILE* decrypted_pkg = fdopen(decrypted_fd, "w+");

		hard_decrypt_package(package, decrypted);

		fclose(pkg);

		pkg = decrypted_pkg;
	}

	verbose_msg(gettext("Creating directory..."));
	if (mkdir(directory, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
		if (errno == EEXIST) 
			errno = 0;
		else {
			warn(gettext("Failed to create directory: %s: %s"), directory, strerror(errno));
			fclose(pkg);
			error(1);
		}
	}
	chdir(directory);

	FILE* metadata = fopen("METADATA", "w");
	FILE* payload  = fopen("PAYLOAD.tar.zst", "w");
	
	pointer(gettext("Extracting %s..."), package);
	hard_split_pkg(pkg, metadata, payload);

	verbose_msg(gettext("Writing %s/METADATA"), directory);
	fclose(metadata);

	verbose_msg(gettext("Writing %s/PAYLOAD.tar.zst"), directory);
	fclose(payload);

	verbose_msg(gettext("Closing %s"), package);
	fclose(pkg);

	int defer_errcode = 0;
	
	if (!recursive) {
		verbose_msg(gettext("Option '--recursive' was not passed. Exiting"));
		goto finish;
	}
	
	verbose_msg(gettext("Creating directory %s/PAYLOAD..."), directory);
	if (mkdir("PAYLOAD", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) == -1) {
		warn(gettext("Failed to create directory: %s: %s"), directory, strerror(errno));
		defer_errcode = 1;
		goto finish;
	}
	chdir("PAYLOAD");

	struct archive *a;	
	struct archive *ext;
	struct archive_entry *entry;
  	int flags;
	int r;

  	flags = ARCHIVE_EXTRACT_TIME;
  	flags |= ARCHIVE_EXTRACT_PERM;
  	flags |= ARCHIVE_EXTRACT_ACL;
  	flags |= ARCHIVE_EXTRACT_FFLAGS;
	
  	a = archive_read_new();
  	archive_read_support_format_tar(a);
	//archive_read_support_format_raw(a);
 	archive_read_support_filter_zstd(a);
  	ext = archive_write_disk_new();
 	archive_write_disk_set_options(ext, flags);
 	archive_write_disk_set_standard_lookup(ext);

	pointer(gettext("Extracting %s/PAYLOAD.tar.zst..."), directory);

	if ((r = archive_read_open_filename(a, "../PAYLOAD.tar.zst", 10240))) {
		warn(gettext("Failed to read PAYLOAD.tar.zst"));
		defer_errcode = 1;
		goto finish;
	}
    		
  	for (;;) {
   		r = archive_read_next_header(a, &entry);
   		if (r == ARCHIVE_EOF)
      			break;
    		if (r < ARCHIVE_OK) {
      			warn(gettext("PAYLOAD.tar.zst: %s"), archive_error_string(a));
		}
		if (r < ARCHIVE_WARN) {
      			warn(gettext("Fatal error occured while extracting PAYLOAD.tar.zst"));
			defer_errcode = 1;
			goto finish;
		}
    		r = archive_write_header(ext, entry);
    		if (r < ARCHIVE_OK) {
    			warn(gettext("PAYLOAD.tar.zst: %s"), archive_error_string(ext));
		}
		else if (archive_entry_size(entry) > 0) {
      			r = copy_data(a, ext);
      			if (r < ARCHIVE_OK) {
				warn(gettext("PAYLOAD.tar.zst: %s"), archive_error_string(ext));
			}
			if (r < ARCHIVE_WARN) {
				warn(gettext("Fatal error occured while extracting PAYLOAD.tar.zst"));
				defer_errcode = 1;
				goto finish;
			}
    		}
		verbose_msg("%s", archive_entry_pathname(entry));
    		r = archive_write_finish_entry(ext);
    		if (r < ARCHIVE_OK) {
			warn(gettext("PAYLOAD.tar.zst: %s"), archive_error_string(ext));
		}
		if (r < ARCHIVE_WARN) {
				warn(gettext("Fatal error occured while extracting PAYLOAD.tar.zst"));
				defer_errcode = 1;
				goto finish;
		}
  	}
  	archive_read_close(a);
  	archive_read_free(a);
  	archive_write_close(ext);
  	archive_write_free(ext);
	chdir("..");
	verbose_msg(gettext("Removing %s/PAYLOAD.tar.zst"), directory);
	unlink("PAYLOAD.tar.zst");
finish:	
	if (decrypted)
		unlink(decrypted);
	if (defer_errcode != 0) {
		error(defer_errcode);
	} else {
		exit(0);
	}
}

static int
copy_data(struct archive *ar, struct archive *aw)
{
  int r;
  const void *buff;
  size_t size;
  la_int64_t offset;

  for (;;) {
    r = archive_read_data_block(ar, &buff, &size, &offset);
    if (r == ARCHIVE_EOF)
      return (ARCHIVE_OK);
    if (r < ARCHIVE_OK)
      return (r);
    r = archive_write_data_block(aw, buff, size, offset);
    if (r < ARCHIVE_OK) {
      	warn("PAYLOAD.tar.zst: %s", archive_error_string(aw));
	return (r);
    }
  }
}

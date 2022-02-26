/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been received a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <libhrd.h> 
#include <errno.h>

#define HARD_NOT_A_PACKAGE 0
#define HARD_ENCRYPTED_PACKAGE 1 
#define HARD_PLAIN_PACKAGE 2

char hard_package_type;

FILE* hard_open_package(char* filename) {
	FILE* 		package;
	unsigned char 	magic[10];
	bool  		dot_hard_suffix;
	bool  		is_pgp_encrypted = false;

	package = fopen(filename, "r");

	if (package == NULL)
		return NULL;

	dot_hard_suffix = (strcmp(filename + (strlen(filename) - 5), ".hard") == 0);

	fread(magic, 10, 1, package);
	rewind(package);

	if (magic[0] == 163 &&
	    magic[1] == 1)
		is_pgp_encrypted = true;
	else if (!strcmp(magic, "METADATA"))
		hard_package_type = HARD_PLAIN_PACKAGE;
	else
		hard_package_type = HARD_NOT_A_PACKAGE;
	
	if (is_pgp_encrypted && dot_hard_suffix)
		hard_package_type = HARD_ENCRYPTED_PACKAGE;

	if (hard_package_type) {
		return package;
	} else {
		errno = EINVAL;
		fclose(package);
		return NULL;
	}
}

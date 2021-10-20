/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libhrd.h>

#include "libhpm.h"

char** hpm_get_package_file_list(package_entry_t* pkg_info){
	char* path = malloc(
			strlen(HPKG_INST_FILES)		/* "/var/hpkg/.../"	*/
			+ strlen(pkg_info->name)	/* package name		*/ 
			+ 1 				/* '-' 			*/
			+ strlen(pkg_info->version) 	/* package version	*/
			+ 5				/* '.list'		*/
			+ 1				/* '\0' 		*/
			);
	strcpy(path, HPKG_INST_FILES);			/* same as above 	*/
	strcat(path, pkg_info->name);
	strcat(path, "-");
	strcat(path, pkg_info->version);
	strcat(path, ".list");

	FILE* file_list = fopen(path, "r");
	free(path);

	if (!file_list) {
		free(path);
		errno = ENOPKG;
		return NULL;
	}
	
	fseek(file_list, 0, SEEK_END);
	size_t file_len = ftell(file_list);
	rewind(file_list);

	char* buffer = malloc(file_len + 1);
	memset(buffer, 0, file_len + 1);

	fread(buffer, 1, file_len, file_list);
	buffer[file_len] = '\0';

	fclose(file_list);

	char** result = hrd_string_split(buffer, "\n");
	
	free(buffer);
	return result;
}

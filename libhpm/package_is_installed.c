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

#include "libhpm.h"

bool hpm_is_package_installed(char* package){
	char* path;
        struct stat file_stats;
	int ret;

	path = malloc(strlen(HPKG_PACKAGES) + strlen(package) + 1);
	strcpy(path, HPKG_PACKAGES);
	strcat(path, package);
	
	ret = stat(path, &file_stats);
	free(path);
	if (ret == 0)	return true;
       	else		return false;	
}

/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been received a copy of license alongside this file (see COPYING)
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
#include <sys/types.h>
#include <dirent.h>

#include <libhrd.h>

#include "libhpm.h"

char** hpm_get_package_list(int ctx) {
	switch (ctx) {
		case HPM_LOCAL_CTX:
			errno = ENOSYS;
			return NULL;
			TODO(Implement support for local packages)
			break;
		case HPM_SYSTEM_CTX:
			int i = 0;
			char** package_list = NULL;
			DIR* pkg_dir = opendir(HPKG_PACKAGES);
			
			if (!pkg_dir)
				return package_list;
			
			struct dirent* pkg = readdir(pkg_dir);

			while (pkg != NULL) {
				if (*(pkg->d_name) == '.') {
					pkg = readdir(pkg_dir);
					continue;
				}
				package_list = realloc(package_list, sizeof(char*) * (i + 1));
				package_list[i] = malloc(strlen(pkg->d_name) + 1);
				strcpy(package_list[i], pkg->d_name);
				pkg = readdir(pkg_dir);
				i++;
			}

			closedir(pkg_dir);

			package_list = realloc(package_list, sizeof(char*) * (i + 1));
			package_list[i] = NULL;

			return package_list;

			break;
		default:
			errno = EINVAL;
			return NULL;
	}
	/* UNREACHABLE */
	return NULL;
}

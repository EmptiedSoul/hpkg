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

package_entry_t* hpm_get_package_info(char* package, int context){
	if (context == HPM_LOCAL_CTX) {
		errno = ENOSYS;
		return NULL;
	}
	package_entry_t* pkg_info = malloc(sizeof(package_entry_t));
	
	char* path = malloc(strlen(HPKG_PACKAGES) + strlen(package) + 1);
	strcpy(path, HPKG_PACKAGES);
	strcat(path, package);

	FILE* config = fopen(path, "r");
	if (!config) {
		free(path);
		return NULL;
	}

	pkg_info->name = malloc(strlen(package)+1);
	strcpy(pkg_info->name, package);

	hrd_config* cfg = hrd_cfg_read(config);

	pkg_info->version = hrd_cfg_get_string(cfg, NULL, "VERSION");
	pkg_info->description = hrd_cfg_get_string(cfg, NULL, "DESCRIPTION");
	pkg_info->maintainer = hrd_cfg_get_string(cfg, NULL, "MAINTAINER");
	
	char* deps = hrd_cfg_get_string(cfg, NULL, "DEPENDS");
	char* confs = hrd_cfg_get_string(cfg, NULL, "CONFLICTS");

	fclose(config);

	hrd_string_discard_chars(deps, '(');
	hrd_string_discard_chars(deps, ')');

	hrd_string_discard_chars(confs, '(');
	hrd_string_discard_chars(confs, ')');

	pkg_info->dependencies = hrd_string_split(deps, " ");
	pkg_info->conflicts = hrd_string_split(confs, " ");

	free(deps);
	free(confs);

	hrd_string_discard_chars(pkg_info->version, '"');
	hrd_string_discard_chars(pkg_info->maintainer, '"');
	hrd_string_discard_chars(pkg_info->description, '"');

	free(path);
	return pkg_info;
}

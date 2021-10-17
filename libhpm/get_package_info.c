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

package_entry_t* hpm_get_package_info(char* package){
	package_entry_t* pkg_info = malloc(sizeof(package_entry_t));
	
	char* path = malloc(strlen(HPKG_PACKAGES) + strlen(package) + 1);
	strcpy(path, HPKG_PACKAGES);
	strcat(path, package);

	pkg_info->name = malloc(strlen(package)+1);
	strcpy(pkg_info->name, package);

	/*
	 * TODO: 
	 * 	Remove checks from hrd_cfg_get_string
	 * 	and place them here
	 */

	pkg_info->version = hrd_cfg_get_string(path, "VERSION");
	pkg_info->description = hrd_cfg_get_string(path, "DESCRIPTION");
	pkg_info->maintainer = hrd_cfg_get_string(path, "MAINTAINER");
	
	char* deps = hrd_cfg_get_string(path, "DEPENDS");
	char* confs = hrd_cfg_get_string(path, "CONFLICTS");

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

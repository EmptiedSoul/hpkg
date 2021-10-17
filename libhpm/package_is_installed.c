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

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

void hpm_free_package_info(package_entry_t* pkg){
	free(pkg->name);
	free(pkg->version);
	free(pkg->description);
	free(pkg->maintainer);
	hrd_array_free((void**)pkg->dependencies);
	hrd_array_free((void**)pkg->conflicts);
	free(pkg);
}

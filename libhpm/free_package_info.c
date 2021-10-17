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

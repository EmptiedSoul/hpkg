#ifndef LIB_HARDCLANZ_PACKAGE_MANAGEMENT_H 
#define LIB_HARDCLANZ_PACKAGE_MANAGEMENT_H

#include <stdbool.h>

#define HPKG_PACKAGES 	"/var/hpkg/packages/"
#define HPKG_INST_FILES "/var/hpkg/installed-files/"

typedef struct _hrd_package_entry {
	char* name;
	char* version;
	char** dependencies;
	char** conflicts;
	char* description;
	char* maintainer;
} package_entry_t;

extern bool hpm_is_package_installed(char* package);
extern package_entry_t* hpm_get_package_info(char* package);
extern char** hpm_get_package_file_list(package_entry_t* pkg_info);
#define hpm_free_package_file_list(file_list) hrd_array_free((void**)file_list)
extern void hpm_free_package_info(package_entry_t* pkg);

#endif 

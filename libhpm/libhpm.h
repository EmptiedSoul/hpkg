/*
 * Copyright (c) 2021-2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been received a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#ifndef LIB_HARDCLANZ_PACKAGE_MANAGEMENT_H 
#define LIB_HARDCLANZ_PACKAGE_MANAGEMENT_H

#include <stdbool.h>
#include <libhrd.h>

#define HPKG_PACKAGES 		"/var/hpkg/packages/"
#define HPKG_INST_FILES 	"/var/hpkg/installed-files/"
#define HARDMAN_MODULES 	"/usr/lib/hardman/modules/"
#define HARDMAN_MODULES_CONFIG	"/usr/lib/hardman/mod-cfg/"
#define HARDMAN_ENABLED_MODULES	HARDMAN_MODULES "enabled/"

typedef struct _hpm_package_entry {
	char* name;
	char* version;
	char** dependencies;
	char** conflicts;
	char* description;
	char* maintainer;
	void* extra;
} package_entry_t;

typedef struct _hpm_module {
	char* name;
	char* version;
	char* textdomain;
	/**/
	char** dependencies;
	char** function_list;
	hrd_hashmap* functions;
} hpm_module;

extern hrd_hashmap* hpm_loaded_modules;

typedef struct _hpm_hardman_cmd_result {
	int status;
	char* output;
} hardman_cmd_result;

enum {
	HPM_MODULE_LOAD,
	HPM_MODULE_CONFIG_ONLY,
};

enum {
	HPM_SYSTEM_CTX,
	HPM_LOCAL_CTX,
};

extern void* hpm_get_module_function(char* module, char* function);
extern hpm_module* hpm_load_module(char* module, int mode);
extern void hpm_module_free(hpm_module* mod);
extern bool hpm_is_package_installed(char* package, int context);
extern package_entry_t* hpm_get_package_info(char* package, int context);
extern char** hpm_get_package_file_list(package_entry_t* pkg_info);
#define hpm_free_package_file_list(file_list) hrd_array_free((void**)file_list)
extern void hpm_free_package_info(package_entry_t* pkg);

extern char** hpm_get_package_list(int context);
#define hpm_free_package_list(list) hrd_array_free((void**)list)

extern DEPRECATED hardman_cmd_result* hpm_run_hardman_cmd(char* cmd, char** args);
extern DEPRECATED void hpm_free_cmd_result(hardman_cmd_result* cmd_result);

#endif 

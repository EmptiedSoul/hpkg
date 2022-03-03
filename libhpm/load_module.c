/*
 * Copyright (c) 2022 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been received a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */
#define _GNU_SOURCE

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>

#include <libhrd.h>

#include "libhpm.h"

hrd_hashmap* hpm_loaded_modules = NULL;

void* hpm_get_module_function(char* module, char* function) {
	if (!hpm_loaded_modules)
		return NULL;
	hpm_module* mod = hrd_hashmap_get_value(hpm_loaded_modules, module);
	return mod ? hrd_hashmap_get_value(mod->functions, function) : NULL;
}

hpm_module* hpm_load_module(char* module, int mode) {
	if (!hpm_loaded_modules)
		hpm_loaded_modules = hrd_hashmap_create(0);
	
	char* mod_lib;
	char* mod_cfg;

	asprintf(&mod_cfg, "%s%s.cfg", HARDMAN_MODULES_CONFIG, module);
	asprintf(&mod_lib, "%s%s.so",  HARDMAN_MODULES, module);
	
	hrd_config* cfg = hrd_cfg_read_at(mod_cfg);
	free(mod_cfg);
	
	char* version		 	= hrd_cfg_get_string(cfg, "Hardman Module", "version");
	char* mod_textdomain		= hrd_cfg_get_string(cfg, "Hardman Module", "textdomain");
	char* dependencies_string   	= hrd_cfg_get_string(cfg, "Hardman Module", "depends");
	char* exported_funcs_string 	= hrd_cfg_get_string(cfg, "Hardman Module", "exported_functions");

	hrd_cfg_free(cfg);

	if (!version || !exported_funcs_string)
		goto read_failed;

	hpm_module* mod = malloc(sizeof(hpm_module));

	char** dependencies = NULL;
	
	if (dependencies_string)
		dependencies = hrd_string_split(dependencies_string,   " ,");
	char** exported_funcs = hrd_string_split(exported_funcs_string, " ,");
	
	mod->name		= strdup(module);
	mod->version		= strdup(version);
	mod->textdomain		= strdup(mod_textdomain);
	mod->dependencies	= dependencies_string ? dependencies : NULL;
	mod->function_list	= exported_funcs;
	mod->functions		= NULL;

	free(exported_funcs_string);
	free(dependencies_string);

	if (mode == HPM_MODULE_CONFIG_ONLY) {
		free(mod_lib);
		return mod;
	}

	if (dependencies) {
		hrd_string_array_foreach (i, dependencies) {
			if (!hrd_hashmap_get_value(hpm_loaded_modules, dependencies[i]))
				if (!hpm_load_module(dependencies[i], mode))
					goto dep_fail;
		}
	}

	void* handle = dlopen(mod_lib, RTLD_LAZY | RTLD_GLOBAL);
	free(mod_lib);

	if (!handle)
		goto dlopen_fail;
	
	hrd_hashmap* functions = hrd_hashmap_create(0);

	hrd_string_array_foreach (i, exported_funcs) {
		hrd_hashmap_set_value(functions,
				exported_funcs[i],
				dlsym(handle, exported_funcs[i]));
	}

	mod->functions = functions;
	
	hrd_hashmap_set_value(hpm_loaded_modules, mod->name, mod);

	return mod;

read_failed:
	free(mod_lib);
	errno = ENOENT;
	return NULL;
dep_fail:
	free(mod_lib);
	free(mod);
	hrd_string_array_free((void*)exported_funcs);
	hrd_string_array_free((void*)dependencies);
	errno = ELIBACC;
	return NULL;
dlopen_fail:
	free(mod);
	if (dependencies)
		hrd_string_array_free((void*)dependencies);
	hrd_string_array_free((void*)exported_funcs);
	return NULL;	
}

void hpm_module_free(hpm_module* mod) {
	free(mod->name);
	free(mod->version);
	free(mod->textdomain);
	hrd_array_free((void*)mod->dependencies);
	hrd_array_free((void*)mod->function_list);
	if (mod->functions)
		hrd_hashmap_free(mod->functions);
}

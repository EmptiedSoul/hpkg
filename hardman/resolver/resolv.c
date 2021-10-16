/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg/hardman
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 *
 * Hardman Simple Fast Dependency RESOLVer (hsfd-resolv)
 */

#include "hsfd-resolv.h"
int main(int argc, char* argv[]){
	for (int i = 1; i<argc; i++){
		query_add_package(argv[i]);					
	}
	for (int i = 0; i<query.amount; i++){
		(void)package_can_be_installed(query.package[i]);
		char* dependencies = package_get_dependencies_string(query.package[i]);
		if (dependencies == NULL) continue;
		char* dependency   = strtok(dependencies, " ");
		while (dependency != NULL){
			if (!package_is_installed(dependency) && !package_is_in_query(dependency))
				query_add_package(dependency);
			dependency = strtok(NULL, " ");
		}
		free(dependencies);
	}
	query_print_packages();
	return RESOLV_SUCCESS;
}

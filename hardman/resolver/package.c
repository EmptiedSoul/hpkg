/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg/hardman
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/license>
 *
 * Hardman Simple Fast Dependency RESOLVer (hsfd-resolv)
 */

#include "hsfd-resolv.h"



bool package_is_installed(char* package_name){
	static const char* path_stub = "/var/hpkg/packages/%s";
	struct stat* buffer = malloc(sizeof(struct stat));
	char* filename = malloc(strlen(path_stub)+strlen(package_name)+1);
	sprintf(filename, path_stub, package_name);
	int retval = stat(filename, buffer);
	free(buffer);
	free(filename);
	if (retval == 0)
		return true;
	else
		return false;
}

bool package_is_in_query(char* package_name){
	for (int i = 0; i<query.amount; i++){
		if (strcmp(query.package[i], package_name) == 0)
			return true;
	}
	return false;
}

char* package_get_dependencies_string(char* package_name){
	static const char* path_stub = "/var/hardman/repo/%s";
	char* filename = malloc(strlen(path_stub)+strlen(package_name)+1);
	sprintf(filename, path_stub, package_name);
	FILE* metadata = fopen(filename, "r");
	free(filename);
	if(metadata == NULL) {
		fprintf(stderr,"resolver: missing entry for package %s in hardman database. Aborting\n", package_name);
		exit(EXIT_FAILURE);
	}	
	char* buffer = malloc(4097);
	while(!feof(metadata)){
		fgets(buffer, 4096, metadata);
		char* token = strtok(buffer, "=");
		if (strcmp(token, "DEPENDS") == 0) {
			token = strtok(NULL, "=");
			char* src, *dst;
			for (src = dst = token; *src != '\0'; src++){
				*dst = *src;
				if (*dst != '(' && *dst != ')' && *dst != '\n') dst++;
			}
			*dst = '\0';
			if (*token == '\0') {
				free(buffer);
				return NULL;
			}
			char* result = strdup(token);
			free(buffer);
			return result;
		}
	}
	return NULL;
}

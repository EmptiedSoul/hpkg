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

Query query;

void query_add_package(char* package_name){
	query.package = realloc(query.package, sizeof(char*)*(query.amount+1));
	query.package[query.amount] = malloc(strlen(package_name)+1);
	strcpy(query.package[query.amount], package_name);
	query.amount++;
}

void query_print_packages(void){
	for (int i = query.amount-1; i>=0; i--){ 
		printf("%s ", query.package[i]); 
	}
	puts("");
}

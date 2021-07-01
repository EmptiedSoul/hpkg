/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg/hardman
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/license>
 *
 * Hardman Simple Fast Dependency RESOLVer (hsfd-resolv)
 */

/* includes */
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* definitions */
typedef struct _Query {
	char**	package;
	int	amount;
} Query;

enum {
	RESOLV_SUCCESS		= EXIT_SUCCESS,
	RESOLV_NOENT		= 1,
	RESOLV_RCONFLICTS	= 2,
	RESOLV_CONFLICT		= 4,	
};

/* prototypes */
extern Query query;

void query_add_package(char* package_name);
void query_print_packages(void);
bool package_is_installed(char* package_name);
bool package_is_in_query(char* package_name);
bool package_can_be_installed(char* package_name);
char* package_get_conflicts_string(char* package_name);
char* package_get_dependencies_string(char* package_name);

/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#ifndef LIBHARD_H
#define LIBHARD_H

#define HARD_NOT_A_PACKAGE 	0
#define HARD_ENCRYPTED_PACKAGE	1
#define HARD_PLAIN_PACKAGE 	2

extern char hard_package_type;

extern FILE* hard_open_package(char* filename);
extern void  hard_split_pkg(FILE* package, FILE* metadata, FILE* payload);
extern int   hard_decrypt_package(char* package, char* decrypted_file);

#endif

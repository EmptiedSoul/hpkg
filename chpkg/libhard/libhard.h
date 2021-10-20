/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#ifndef LIBHARD_H
#define LIBHARD_H

extern bool hard_is_hard(char* filename);
extern void hard_split_pkg(FILE* package, FILE* metadatam, FILE* payload);
extern int  hard_decrypt_package(char* package, char* decrypted_file);
extern bool hard_is_package_encrypted(FILE* package);

#endif

/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

/* We dont actually validate package structure (only .hard suffix), if its incorrect we will fail at decryption */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <libhrd.h> 
#include <errno.h>

bool hard_is_hard(char* package){
	if (!hrd_file_exist(package, HRDFS_ISFILE | HRDFS_READBL)){
		errno = ENOENT;
		return false;
	}
	if (strcmp(package+(strlen(package)-5), ".hard") == 0)
		return true;
	else {
		errno = EINVAL;
		return false;
	}
}


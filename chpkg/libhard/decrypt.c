/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int hard_decrypt_package(char* package, char* decrypted_file){
	
	pid_t pid;
	int status;

	switch(pid = fork()){
		case -1:
			perror("fork");
			return -1;
			break;
		case 0:
			execlp("gpg", "gpg", "-d", package, "-o", decrypted_file, NULL);
			break;
		default:
			wait(&status);
			if (WIFEXITED(status))
				return WEXITSTATUS(status);
			errno = EBADMSG;
			return -1;
	}
	
	return -1;
}

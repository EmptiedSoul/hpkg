/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

#include <libhrd.h>

#include "libhpm.h"

hardman_cmd_result* hpm_run_hardman_cmd(char* cmd, char* args[]){
	int status;
	int argc = 0;
	
	for (; args[argc]; argc++);
	argc += 2;
	
	char** _args = (char**)malloc(sizeof(char*) * argc);
	_args[0] = "hardman";
	_args[1] = cmd;
	
	for (int i = 2; i < argc; i++){
		_args[i] = args[i - 2];
	}
	_args[argc] = NULL; 

	int fd[2];
	pipe(fd);

	switch (fork()) {
	case -1:
		perror("hpm_run_hardman_cmd: fork");
		free(_args);
		return NULL;
		break;
	case 0: /* Child */
		close(fd[0]);
		close(0);
		dup2(fd[1], 1);
		dup2(fd[1], 2);
		execvp("hardman", _args);
		/* UNREACHABLE */
		perror("exec");
		return NULL;
		break;
	default:
		wait(&status);
		char* buf = (char*)malloc(4096 + 2);
		free(_args);
		close(fd[1]);
		read(fd[0], buf, 4096);
		buf[4096] = '\0';
		hardman_cmd_result* result = malloc(sizeof(hardman_cmd_result));
		result->status = WEXITSTATUS(status);
		result->output = buf;
		return result;
	}
	return NULL;
}

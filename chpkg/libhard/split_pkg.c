/*
 * Copyright (c) 2021 Arseniy 'Emptied Soul' Lesin
 * This file is part of hpkg
 * Licensed under terms of GPLv3 
 * You should been recieved a copy of license alongside this file (see COPYING)
 * if not - see <http://gnu.org/licenses>
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/sendfile.h>

#define BUF_SIZE 512

void hard_split_pkg(FILE* package, FILE* metadata, FILE* payload){
	char* string = NULL;
	off_t offset = 0;
	(void) fscanf(package, "%m[^\n]\n", &string); /* skip 1st line */
	
	for (;;){
		free(string);
		string = NULL;
		offset = ftell(package);
		(void) fscanf(package, "%m[^\n]\n", &string);
		if (*string == '\0') break;
 		fputs(string, metadata);
		fputs("\n", metadata);
	}
	free(string);
	string = NULL;

	if (payload == NULL) return;

	fseek(package, offset, SEEK_SET);

	char* buffer = malloc(BUF_SIZE);

	buffer[0] = '\0';

	while (buffer[0] == '\0'){
		fread(buffer, 1, 1, package);
	}
		
	fseek(package, 123, SEEK_CUR);

	char size[12];
	fread(size, 12, 1, package);

	off_t payload_size = strtol(size, NULL, 8);

	fseek(package, 512-136, SEEK_CUR);
	off_t cur_pos = ftell(package);

	sendfile(fileno(payload), fileno(package), &cur_pos, payload_size);

	free(buffer);

	return;
}

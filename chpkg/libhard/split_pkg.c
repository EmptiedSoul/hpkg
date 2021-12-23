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

#define BUF_SIZE		4096
#define SIZE_FIELD_OFFSET	124

void hard_split_pkg(FILE* package, FILE* metadata, FILE* payload){
	size_t 	metadata_size;
	size_t 	payload_size;
	off_t  	payload_offset;
	off_t	metadata_offset	= 512;
	char	size_field[12];

	/* Seek size field in our POSIX tar header */
	fseek(package, SIZE_FIELD_OFFSET, SEEK_SET); 
	fread(size_field, 12, 1, package);
	metadata_size = strtol(size_field, NULL, 8); /* size represented by ASCII octal numbers*/
	
	if (!payload) {
		rewind(package);
		sendfile(fileno(metadata), fileno(package), &metadata_offset, metadata_size);
		return;
	}

	payload_offset = metadata_offset + metadata_size;
	payload_offset += payload_offset % 512; /* padding */
	payload_offset += 512; /* tar header */
	
	/* Seek next header */
	fseek(package, payload_offset - 512 + SIZE_FIELD_OFFSET, SEEK_SET);
	fread(size_field, 12, 1, package);
	payload_size = strtol(size_field, NULL, 8);

	rewind(package);

	sendfile(fileno(metadata), fileno(package), &metadata_offset, metadata_size);
	sendfile(fileno(payload), fileno(package), &payload_offset, payload_size);
}

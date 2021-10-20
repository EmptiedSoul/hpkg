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

#include <libhrd.h>

#include "libhpm.h"

void hpm_free_cmd_result(hardman_cmd_result* cmd_result){
	if (!cmd_result) return;
	if (!cmd_result->output)
		free(cmd_result->output);
	free(cmd_result);
}

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

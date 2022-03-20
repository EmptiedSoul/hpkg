#ifndef HARDMAN_VARIABLES_H
#define HARDMAN_VARIABLES_H

typedef enum _batch_mode {
	BATCH_DEF,
	BATCH_NO,
	BATCH_YES,
} batch_mode_t;

extern batch_mode_t batch_mode;

extern int verbose_lvl;
extern int silent_lvl;

extern hrd_hashmap* hardman_commands;
extern hrd_config*  hardman_config;
extern hrd_hashmap* hardman_mod_help;
extern hrd_hashmap* hardman_mod_vers;

#endif

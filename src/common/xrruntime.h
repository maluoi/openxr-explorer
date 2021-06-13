#pragma once

#include <stdint.h>
#include <stdbool.h>

/*** Types *******************************/

typedef enum platform_ {
	platform_none,
	platform_windows,
	platform_linux,
} platform_;

typedef struct runtime_t {
	platform_ platform;
	char      name[64];
	char      file[1024];
	bool      present;
} runtime_t;

/*** Signatures **************************/

bool load_runtimes(const char *file, runtime_t **out_runtime_list, int32_t *out_runtime_count);
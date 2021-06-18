#include "xrruntime.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

/*** Signatures **************************/

bool file_exists(const char *file);

/*** Code ********************************/

bool load_runtimes(const char *file, runtime_t **out_runtime_list, int32_t *out_runtime_count) {
	FILE *fp = nullptr;
	fp = fopen(file, "r");
	if (fp == nullptr) {
		printf("Couldn't load %s!\n", file);
		return false;
	}

	runtime_t *result_list  = nullptr;
	int32_t    result_count = 0;

	char line[1024];
	while (fgets(line, sizeof(line), fp)) {
		runtime_t runtime       = {};
		int32_t   spaces        = 0;
		int32_t   curr          = 0;
		char      plat_name[32] = {};
		size_t    len           = strlen(line);
		for (int32_t i=0; i<len; i+=1) {
			if (line[i] == ' ') {
				if (spaces == 0) {
					plat_name[curr] = '\0';
					curr = -1;
				} else if (spaces == 1) {
					runtime.name[curr] = '\0';
					curr = -1;
				} else runtime.file[curr] = line[i];
				spaces += 1;
			} else if (line[i] == '\n') {
				break;
			} else {
				if      (spaces == 0) plat_name   [curr] = line[i];
				else if (spaces == 1) runtime.name[curr] = line[i];
				else                  runtime.file[curr] = line[i];
			}
			curr += 1;
		}
		runtime.file[curr]='\0';

		if      (strcmp(plat_name,"windows") == 0) runtime.platform = platform_windows;
		else if (strcmp(plat_name,"linux"  ) == 0) runtime.platform = platform_linux;

		result_count += 1;
		result_list = (runtime_t*)realloc(result_list, sizeof(runtime_t) * result_count);
		result_list[result_count-1] = runtime;
	}
	fclose(fp);

	#ifdef _WIN32
	platform_ curr_platform = platform_windows;
	#else
	platform_ curr_platform = platform_linux;
	#endif
	for (int32_t i=0; i<result_count; i+=1) {
		result_list[i].present = 
			result_list[i].platform == curr_platform && 
			file_exists(result_list[i].file);
	}

	*out_runtime_list  = result_list;
	*out_runtime_count = result_count;
	return true;
}

///////////////////////////////////////////

bool file_exists(const char *file) {
	struct stat buffer;   
	return (stat (file, &buffer) == 0);
}
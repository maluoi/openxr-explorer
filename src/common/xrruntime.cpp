#include "xrruntime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

/*** Default Runtime File ****************/

#include "xr_runtime_default.h"
#include "xr_runtime_template.h"

/*** Signatures **************************/

bool file_exists(const char *file);

/*** Code ********************************/

bool load_runtimes(const char *file, runtime_t **out_runtime_list, int32_t *out_runtime_count) {
	const char *files[3]  = {};
	FILE       *fp        = nullptr;
	char       *file_data = nullptr;
	fp = fopen(file, "r");
	if (fp != nullptr) {
		// Get length of file
		fseek(fp, 0, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		// Read the data
		file_data = (char*)malloc(size+1);
		fread (file_data, 1, size, fp);
		fclose(fp);

		// Stick an end string 0 character at the end in case the caller wants
		// to treat it like a string
		file_data[size] = 0;

		files[0] = file_data;
		files[1] = runtime_default_list;
	} else {
		files[0] = runtime_default_list;
	}
	

	runtime_t *result_list  = nullptr;
	int32_t    result_count = 0;
	int32_t    file_id = 0;
	const char*line    = files[0];
	while (files[file_id] != nullptr && *line != '\0') {
		runtime_t runtime       = {};
		int32_t   spaces        = 0;
		int32_t   curr          = 0;
		char      plat_name[32] = {};
		for (int32_t i=0; ; i+=1) {
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
				line = &line[i+1];
				break;
			} else if (line[i] == '\0') {
				file_id++;
				line = files[file_id];
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
		else if (strcmp(plat_name,"#") == 0) continue;

		result_count += 1;
		result_list = (runtime_t*)realloc(result_list, sizeof(runtime_t) * result_count);
		result_list[result_count-1] = runtime;
	}
	free(file_data);

	#if defined(_WIN32)
	platform_ curr_platform = platform_windows;
	#elif defined(__linux)
	platform_ curr_platform = platform_linux;

	// On linux, we want to support the '~' path feature, esp. for SteamVR
	const char *user = getenv("USER");
	if (strcmp(user, "root") == 0)
		user = getenv("SUDO_USER");
		
	char path_tmp[1024];
	for (int32_t i=0; i<result_count; i+=1) {
		if (result_list[i].file[0] == '~') {
			snprintf(path_tmp, sizeof(path_tmp), "/home/%s%s", user, &result_list[i].file[1]);
			strcpy(result_list[i].file, path_tmp);
		}
	}
	#endif

	for (int32_t i=0; i<result_count; i+=1) {
		// See if there's an earlier one
		bool exists = false;
		for (size_t e = 0; e < i; e++) {
			if (result_list[e].present && strcmp(result_list[e].name, result_list[i].name) == 0) {
				exists = true;
				break;
			}
		}
		
		result_list[i].present = 
			!exists &&
			result_list[i].platform == curr_platform && 
			file_exists(result_list[i].file);
	}

	*out_runtime_list  = result_list;
	*out_runtime_count = result_count;
	return true;
}

///////////////////////////////////////////

void ensure_runtime_config_exists(const char *at_file) {
	if (file_exists(at_file))
		return;

	FILE *fp = fopen(at_file, "w");
	if (fp != nullptr) {
		fputs(runtime_default_template, fp);
		fclose(fp);
	} else {
		printf("Error creating default config file at %s!\n", at_file);
	}
}

///////////////////////////////////////////

#if defined(_WIN32)

const char *runtime_config_path() {
	return "openxr_explorer_runtimes.txt";
}

#elif defined(__linux__) 

char runtime_config_path_str[1024];
const char *runtime_config_path() {
	const char *config_root = getenv("XDG_CONFIG_HOME");
	if (config_root == nullptr) {
		config_root = getenv("HOME");
		snprintf(runtime_config_path_str, sizeof(runtime_config_path_str), "%s/.config/openxr_explorer_runtimes.txt", config_root);
	} else {
		snprintf(runtime_config_path_str, sizeof(runtime_config_path_str), "%s/openxr_explorer_runtimes.txt", config_root);
	}
	
	printf("Config root: %s\n", runtime_config_path_str);
	
	return runtime_config_path_str;
}

#endif

///////////////////////////////////////////

bool file_exists(const char *file) {
	struct stat buffer;   
	return (stat (file, &buffer) == 0);
}
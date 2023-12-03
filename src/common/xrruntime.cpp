#include "xrruntime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/stat.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#endif

#include <nlohmann/json.hpp>

/*** Default Runtime File ****************/

#include "xr_runtime_default.h"
#include "xr_runtime_template.h"

/*** Signatures **************************/

bool file_exists(const char *file);
char* read_file(const char* file);

/*** Code ********************************/

bool load_runtimes(const char *file, runtime_t **out_runtime_list, int32_t *out_runtime_count) {
	const char *files[3]  = {};
	char       *file_data = read_file(file);
	if (file_data != nullptr) {
		files[0] = file_data;
		files[1] = runtime_default_list;
	} else {
		files[0] = runtime_default_list;
	}
	

	runtime_t *result_list  = nullptr;
	int32_t    result_count = 0;
	int32_t    file_id = 0;
	const char*line    = files[0];
	while (files[file_id] != nullptr) {
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
		else continue;

		result_count += 1;
		result_list = (runtime_t*)realloc(result_list, sizeof(runtime_t) * result_count);
		result_list[result_count-1] = runtime;
	}
	free(file_data);

	#if defined(_WIN32)
	platform_ curr_platform = platform_windows;
	HKEY available_runtimes_key {};
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\OpenXR\\1\\AvailableRuntimes", 0, KEY_READ, &available_runtimes_key) == ERROR_SUCCESS && available_runtimes_key) {
		// The actual limit is 16383 based on the registry limits:
		//   https://learn.microsoft.com/en-us/windows/win32/sysinfo/registry-element-size-limits
		// However, we'll stick to 1024 (including terminating null) as we need to fit in
		// runtime_t::file
		char path[1024];
		// If we end up switching to wchar, this will still be a character count, not a byte count
		DWORD path_char_count = sizeof(path);
		DWORD value_type;
		DWORD disabled;
		DWORD disabled_byte_count = sizeof(disabled);
		for (DWORD i=0; ;++i) {
			auto res = RegEnumValueA(available_runtimes_key, i, path, &path_char_count, nullptr, &value_type, reinterpret_cast<LPBYTE>(&disabled), &disabled_byte_count);
			if (res == ERROR_NO_MORE_ITEMS) {
				break;
			}
			if (res == ERROR_SUCCESS && value_type == REG_DWORD) {
				result_count += 1;
				result_list = (runtime_t*)realloc(result_list, sizeof(runtime_t) * result_count);
				auto& runtime = result_list[result_count - 1];
				runtime.platform = curr_platform;
				runtime.name[0] = 0;
				memcpy(runtime.file, path, path_char_count);
				runtime.file[path_char_count] = '\0';
				runtime.present = file_exists(runtime.file) && !disabled;
			}
			path_char_count = sizeof(path);
			disabled_byte_count = sizeof(disabled);
		}
		RegCloseKey(available_runtimes_key);
	}
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
		auto& runtime = result_list[i];
		if (runtime.name[0] || !file_exists(runtime.file)) {
			continue;
		}

		// Try to read the name rom the JSON
		auto json = read_file(runtime.file);
		if (!json) {
			continue;
		}
		try {
			const auto manifest = nlohmann::json::parse(json);
			free(json);	
			json = nullptr;
			if (manifest.contains("runtime")) {
				const auto& runtime_metadata = manifest.at("runtime");
				if (runtime_metadata.contains("name")) {
					const auto& name = runtime_metadata.at("name");
					if (name.is_string()) {
						auto str = name.get<std::string>();
						// Include trailing null in copy
						if (str.size() + 1 <= sizeof(runtime.name)) {
							memcpy(runtime.name, str.c_str(), str.size() + 1);
						}
					}
				}
			}
		} catch (const nlohmann::json::exception&) {
			if (json) {
				free(json);
			}
		}

		if (runtime.name[0]) {
			continue;
		}

		// "name" is an optional field in the manifest, so might be legitimately be missing,
		// or we might have had a failure. If so, let's try and put *something* there
		auto basename_start = strrchr(runtime.file, '\\');
		auto basename_end = strrchr(runtime.file, '.');
		if (basename_start && basename_end > basename_start) {
			basename_start++;
			memcpy(runtime.name, basename_start, basename_end - basename_start);
			runtime.name[basename_end - basename_start] = '\0';
		} else {
			memcpy(runtime.name, "Unknown", sizeof("Unknown"));
		}
	}

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
	return "runtimes.txt";
}

#elif defined(__linux__) 

char runtime_config_path_str[1024];
const char *runtime_config_path() {
	const char *config_root = getenv("XDG_CONFIG_HOME");
	if (config_root == nullptr) {
		config_root = getenv("HOME");
		snprintf(runtime_config_path_str, sizeof(runtime_config_path_str), "%s/.config/openxr-explorer/runtimes.txt", config_root);
	} else {
		snprintf(runtime_config_path_str, sizeof(runtime_config_path_str), "%s/openxr-explorer/runtimes.txt", config_root);
	}
	
	return runtime_config_path_str;
}

#endif

///////////////////////////////////////////

bool file_exists(const char *file) {
	struct stat buffer;   
	return (stat (file, &buffer) == 0);
}

char* read_file(const char* file) {
	FILE* fp = fopen(file, "r");
	if (!fp) {
		return nullptr;
	}

	// Get length of file
	fseek(fp, 0, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Read the data
	char* file_data = (char*)malloc(size+1);
	char* it = file_data;
	size_t bytes_to_read = size;
	while (size_t bytes_read = fread(it, 1, bytes_to_read, fp)) {
		bytes_to_read -= bytes_read;
		it += bytes_read;
	}
	// Stick an end string 0 character at the end in case the caller wants
	// to treat it like a string
	*it = '\0';
	fclose(fp);
	return file_data;
}
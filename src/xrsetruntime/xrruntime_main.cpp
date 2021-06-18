#include "xrruntime.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*** Global Variables ********************/

runtime_t *runtimes      = nullptr;
int32_t    runtime_count = 0;

/*** Signatures **************************/

void show_runtimes   ();
void show_help       ();
bool set_runtime     (const char *flag);
bool activate_runtime(int32_t index);
int  strcmp_nocase   (char const *a, char const *b);

/*** Code ********************************/

int main(int32_t arg_count, const char **args) {
	if (!load_runtimes("xr_runtimes.txt", &runtimes, &runtime_count))
		return -1;

	if (arg_count < 2) show_help();
	for (int32_t i=1; i<arg_count; i++) {
		if (strcmp(args[i], "--list") == 0 || strcmp(args[i], "-l") == 0 || strcmp(args[i], "/list") == 0) {
			show_runtimes();
		}
		else if (strcmp(args[i], "--help") == 0 || strcmp(args[i], "-h") == 0 || strcmp(args[i], "/?") == 0) {
			show_help();
		}
		else if (!set_runtime(args[i])) {
			show_help();
		}
	}

	return 0;
}

///////////////////////////////////////////

void show_runtimes() {
	for (int32_t i=0; i<runtime_count; i+=1) {
		if (runtimes[i].present)
			printf("%s\n", runtimes[i].name);
	}
	if (runtime_count == 0) {
		printf("No runtime found! Do you have one installed, and is it listed in xrsetruntime's xr_runtimes.txt file?\n");
	}
}

///////////////////////////////////////////

void show_help() {
	printf(R"_(
Usage: xrsetruntime [option]

Options:
	--list      Displays a list of runtimes that are on the system and
	            available. This list can be updated by modifying the 
	            accompanying 'xr_runtimes.txt' file.
	--help      Show this help information!

)_");
	for (int32_t i=0; i<runtime_count; i+=1) {
		if (runtimes[i].present)
			printf("	--%-10sSets %s as the active runtime.\n", runtimes[i].name, runtimes[i].name);
	}
	if (runtime_count == 0) {
		printf("	--[runtime] Make sure you've installed an OpenXR runtime, and it's listed\n	            in the accompanying xr_runtimes.txt file! None were found.\n");
	}
}

///////////////////////////////////////////

bool set_runtime(const char *flag) {
	while (*flag == '-') flag++;
	for (int32_t i = 0; i < runtime_count; i++) {
		if (strcmp_nocase(flag, runtimes[i].name) == 0) {
			if (!runtimes[i].present) {
				printf("%s isn't installed, or xr_runtimes.txt isn't pointing to the right place! [%s]\n", runtimes[i].name, runtimes[i].file);
				return true;
			} else {
				if (activate_runtime(i)) {
					printf("Activated %s!\n", runtimes[i].name);
				}
				return true;
			}
		}
	}
	return false;
}

///////////////////////////////////////////

int32_t strcmp_nocase(char const *a, char const *b) {
	for (;; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a)
			return d;
	}
	return -1;
}

///////////////////////////////////////////

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
bool activate_runtime(int32_t index) {
	// Reference powershell script, does the same thing
	// Set-ItemProperty -Path $key -Name $name -Value $runtime
	// $key     = "HKLM:\SOFTWARE\Khronos\OpenXR\1"
	// $name    = "ActiveRuntime"

	HKEY key;
	LSTATUS result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Khronos\\OpenXR\\1", 0, KEY_SET_VALUE, &key);
	if (result == ERROR_ACCESS_DENIED) {
		printf("Access denied, please run as administrator!\n");
		return false;
	} else if (result != ERROR_SUCCESS) {
		printf("Failed to open registry key for unknown reason! [%d]\n", (int32_t)result);
		return false;
	}

	if (RegSetValueEx(key, "ActiveRuntime", 0, REG_SZ, (LPBYTE)runtimes[index].file, (DWORD)(strlen(runtimes[index].file)+1)) != ERROR_SUCCESS) {
		printf("Failed to set registry key!\n");
		RegCloseKey(key);
		return false;
	}

	RegCloseKey(key);
	return true;
}

#elif defined(__linux__)

bool activate_runtime(int32_t index) {
	return false;
}

#endif
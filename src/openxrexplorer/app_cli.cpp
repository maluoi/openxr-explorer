#include "app_cli.h"
#include "array.h"
#include "openxr_info.h"

#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

/*** Types *******************************/

struct command_t {
	const char *name_func;
	const char *name_type;
	bool        requires_session;
	void      (*show)();
};

/*** Signatures **************************/

void cli_print_table(const display_table_t *table);
void cli_show_help();
int32_t strcmp_nocase(char const *a, char const *b);

/*** Code ********************************/

void app_cli(int32_t arg_count, const char **args) {
	xr_settings_t settings = {};
	settings.allow_session = false;
	settings.form          = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

	if (!skg_init("OpenXR Explorer", nullptr))
		printf("Failed to init skg!\n");
	openxr_info_reload(settings);
	if (xr_instance_err) printf("XrInstance error: [%s]\n", xr_instance_err);
	if (xr_system_err)   printf("XrSystemId error: [%s]\n", xr_system_err);
	if (xr_session_err)  printf("XrSession error: [%s]\n", xr_session_err);

	// Find all the commands we want to execute
	bool show = false;
	for (size_t i = 1; i < arg_count; i++) {
		const char *curr = args[i];
		while (*curr == '-') curr++;

		if (strcmp_nocase("help", curr) == 0 || strcmp_nocase("h", curr) == 0 || strcmp_nocase("/h", curr) == 0) {
			cli_show_help();
			show = true;
		} else {
			for (size_t c = 0; c < xr_tables.count; c++) {
				if ((xr_tables[c].name_func && strcmp_nocase(xr_tables[c].name_func, curr) == 0) ||
					(xr_tables[c].name_type && strcmp_nocase(xr_tables[c].name_type, curr) == 0)) {
					cli_print_table(&xr_tables[c]);
					show = true;
					break;
				}
			}
		}
	}
	if (!show)
		cli_show_help();

	openxr_info_release();
	skg_shutdown();
}

///////////////////////////////////////////

void cli_show_help() {
	printf(R"_(
Usage: openxr-explorer [option list...]

Notes:	This tool shows a list of values provided from the active OpenXR
	runtime. If a type is specified, the associated function will be
	called. If a function is specified, the associated data will be 
	shown. Options are case insensitive.

Options:
	-help	Show this help information!

)_");
	printf("	FUNCTIONS\n");
	for (size_t i = 0; i < xr_tables.count; i++) {
		if (xr_tables[i].name_func)
			printf("	-%s\n", xr_tables[i].name_func);
	}
	printf("\n	TYPES\n");
	for (size_t i = 0; i < xr_tables.count; i++) {
		if (xr_tables[i].name_type)
			printf("	-%s\n", xr_tables[i].name_type);
	}
}

///////////////////////////////////////////

void cli_print_table(const display_table_t *table) {
	printf("%s\n", table->show_type ? table->name_type : table->name_func);

	size_t max[3] = {};
	for (size_t i = table->header_row?1:0; i < table->cols[0].count; i++) {
		for (size_t c = 0; c < table->column_count; c++) {
			size_t len = table->cols[c][i].text ? strlen(table->cols[c][i].text) : 0;
			if (max[c] < len)
				max[c] = len;
		}
	}

	for (size_t i = table->header_row ? 1 : 0; i < table->cols[0].count; i++) {
		printf("| ");
		for (size_t c = 0; c < table->column_count; c++) {
			printf("%-*s", max[c], table->cols[c][i].text ? table->cols[c][i].text : "");
			if (c != table->column_count-1)
				printf(" | ");
		}
		printf(" |\n");
	}
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
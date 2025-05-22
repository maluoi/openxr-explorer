#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <stdint.h>

bool app_args(int32_t arg_count, const char **args);
bool app_init();
void app_step(ImVec2 canvas_size);
void app_shutdown();

extern const char *app_name;
extern const char *app_id;
extern const char *app_path_config;
extern float       app_scale;
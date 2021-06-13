#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include <stdint.h>

bool app_args(int32_t arg_count, const char **args);
bool app_init();
void app_step(ImVec2 canvas_size);
void app_shutdown();

extern const char *app_name;
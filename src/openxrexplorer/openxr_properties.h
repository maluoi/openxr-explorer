#pragma once

#include <openxr/openxr.h>

void openxr_load_system_properties(XrInstance instance, XrSystemId system_id, array_t<display_table_t>* tables);
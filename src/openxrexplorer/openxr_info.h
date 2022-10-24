#pragma once

#include "array.h"
#include "imgui/sk_gpu.h"
#if defined(SKG_DIRECT3D11)
#define XR_USE_GRAPHICS_API_D3D11
#elif defined(SKG_OPENGL)
#define XR_USE_GRAPHICS_API_OPENGL
#endif

#if defined(_WIN32)
#define XR_USE_PLATFORM_WIN32
#elif defined(__linux__)
#define XR_USE_PLATFORM_XLIB
#endif

#include <openxr/openxr.h>

/*** Types *******************************/

enum display_tag_ {
	display_tag_none,
	display_tag_properties,
	display_tag_features,
	display_tag_view,
	display_tag_misc,
};

struct display_item_t {
	const char *text;
	const char *spec;
};

struct display_table_t {
	const char             *error;
	const char             *name_type;
	const char             *name_func;
	const char             *spec;
	display_tag_            tag;
	bool                    header_row;
	bool                    show_type;
	int32_t                 column_count;
	array_t<display_item_t> cols[3];
};

struct xr_settings_t {
	XrViewConfigurationType view_config;
	XrFormFactor            form;
	bool                    allow_session;
};

struct xr_enum_info_t {
	const char           *source_fn_name;
	const char           *source_type_name;
	const char           *spec_link;
	const char           *error;
	bool                  requires_instance;
	bool                  requires_system;
	bool                  requires_session;
	display_tag_          tag;
	array_t<const char *> items;
	XrResult            (*load_info)(xr_enum_info_t *ref_info, xr_settings_t settings);
};

struct xr_properties_t {
	XrInstanceProperties                     instance;
	XrSystemProperties                       system;
	XrSystemHandTrackingPropertiesEXT        hand_tracking;
	XrSystemHandTrackingMeshPropertiesMSFT   hand_mesh;
	XrSystemEyeGazeInteractionPropertiesEXT  gaze;
	XrSystemFoveatedRenderingPropertiesVARJO foveated_varjo;
	XrSystemColorSpacePropertiesFB           color_space_fb;
	XrSystemFacialTrackingPropertiesHTC      facial_tracking_htc;
	XrSystemKeyboardTrackingPropertiesFB     keyboard_tracking_fb;
	XrSystemMarkerTrackingPropertiesVARJO    marker_tracking_varjo;
	XrSystemPassthroughPropertiesFB          passthrough_fb;
	XrSystemRenderModelPropertiesFB          render_model_fb;
	XrSystemSpatialEntityPropertiesFB        spatial_entity_fb;
	XrSystemSpaceWarpPropertiesFB            space_warp_fb;
};

struct xr_view_info_t {
	XrViewConfigurationType          current_config;
	array_t<XrViewConfigurationType> available_configs;
	array_t<const char *>            available_config_names;
	XrViewConfigurationProperties    config_properties;
	array_t<XrViewConfigurationView> config_views;
};

/*** Global Variables ********************/

extern array_t<display_table_t> xr_tables;

extern array_t<xr_enum_info_t> xr_misc_enums;
extern xr_properties_t         xr_properties;
extern xr_view_info_t          xr_view;

extern const char *xr_instance_err;
extern const char *xr_session_err;
extern const char *xr_system_err;

extern const char* xr_runtime_name;

/*** Signatures **************************/

void openxr_info_reload (xr_settings_t settings);
void openxr_info_release();
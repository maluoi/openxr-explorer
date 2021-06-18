#include "openxr_info.h"

#if defined(__linux__)
#include <GL/glxew.h>
#endif

#include <openxr/openxr_platform.h>
#include <openxr/openxr_reflection.h>

#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>

/*** Types *******************************/

struct xr_extensions_t {
	array_t<XrExtensionProperties> extensions;
	array_t<XrApiLayerProperties>  layers;
};

/*** Global Variables ********************/

array_t<display_table_t> xr_tables        = {};
array_t<char *>          xr_table_strings = {};

array_t<xr_enum_info_t> xr_misc_enums = {};
xr_properties_t         xr_properties = {};
xr_view_info_t          xr_view       = {};
xr_extensions_t         xr_extensions = {};

XrInstance  xr_instance     = {};
const char *xr_instance_err = nullptr;
XrSession   xr_session      = {};
const char *xr_session_err  = nullptr;
XrSystemId  xr_system_id    = {};
const char *xr_system_err   = nullptr;

/*** Signatures **************************/

void openxr_init_instance(array_t<XrExtensionProperties> extensions);
void openxr_init_system  (XrFormFactor form);
void openxr_init_session ();

xr_extensions_t openxr_load_exts      ();
xr_properties_t openxr_load_properties();
xr_view_info_t  openxr_load_view      (XrViewConfigurationType view_config);
void            openxr_load_enums     (xr_settings_t settings);
const char *    openxr_result_string  (XrResult result);
void            openxr_register_enums();


/*** Code ********************************/

void openxr_info_reload(xr_settings_t settings) {
	openxr_info_release();

	xr_extensions = openxr_load_exts();
	openxr_init_instance(xr_extensions.extensions);
	openxr_init_system  (settings.form);
	xr_properties = openxr_load_properties();
	xr_view       = openxr_load_view      (settings.view_config);

	openxr_register_enums();
	openxr_load_enums    (settings);

	if (xr_session) {
		xrDestroySession(xr_session);
		xr_session = XR_NULL_HANDLE;
	}
}

///////////////////////////////////////////

void openxr_info_release() {
	xr_misc_enums.each([](xr_enum_info_t &i) { i.items.free(); });
	xr_misc_enums.free();
	xr_properties = {};
	xr_view.available_configs     .free();
	xr_view.available_config_names.free();
	xr_view.config_views          .free();
	xr_view = {};
	xr_extensions.extensions.free();
	xr_extensions.layers    .free();
	xr_extensions = {};

	xr_table_strings.each(free);
	xr_table_strings.free();
	xr_tables.each([](display_table_t &t) {for (int32_t i=0; i<t.column_count; i++) t.cols[i].free(); });
	xr_tables.free();

	if (xr_session)  xrDestroySession (xr_session);
	if (xr_instance) xrDestroyInstance(xr_instance);

	xr_session      = XR_NULL_HANDLE;
	xr_instance     = XR_NULL_HANDLE;
	xr_system_id    = XR_NULL_SYSTEM_ID;
	xr_session_err  = nullptr;
	xr_instance_err = nullptr;
	xr_system_err   = nullptr;
}

///////////////////////////////////////////

const char *openxr_result_string(XrResult result) {
	switch (result) {
#define ENTRY(NAME, VALUE) \
	case VALUE: return #NAME;
		XR_LIST_ENUM_XrResult(ENTRY)
#undef ENTRY
	default: return "<UNKNOWN>";
	}
}

///////////////////////////////////////////

const char *new_string(const char *format, ...) {
	va_list args;
	char   *result;

	va_start(args, format);
	size_t len = vsnprintf(0, 0, format, args);
	va_end(args);
	if ((result = (char*)malloc(len + 1)) != 0) {
		va_start(args, format);
		vsnprintf(result, len+1, format, args);
		va_end(args);
		xr_table_strings.add(result);
		return result;
	} else {
		return "";
	}
}

///////////////////////////////////////////

void openxr_init_instance(array_t<XrExtensionProperties> extensions) {
	if (xr_instance != XR_NULL_HANDLE || xr_instance_err != nullptr)
		return;

	array_t<const char *> exts = {};
#if defined(XR_USE_GRAPHICS_API_OPENGL)
	exts.add(XR_KHR_OPENGL_ENABLE_EXTENSION_NAME);
#elif defined(XR_USE_GRAPHICS_API_OPENGL_ES)
	exts.add(XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME);
#elif defined(XR_USE_GRAPHICS_API_D3D11)
	exts.add(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
#endif

	exts.clear();
	for (size_t i = 0; i < extensions.count; i++) {
		exts.add(extensions[i].extensionName);
	}

	XrInstanceCreateInfo create_info = { XR_TYPE_INSTANCE_CREATE_INFO };
	create_info.enabledExtensionCount = exts.count;
	create_info.enabledExtensionNames = exts.data;
	create_info.enabledApiLayerCount  = 0;
	create_info.enabledApiLayerNames  = nullptr;
	create_info.applicationInfo.applicationVersion = 1;
	create_info.applicationInfo.engineVersion      = 1;
	create_info.applicationInfo.apiVersion         = XR_CURRENT_API_VERSION;
	snprintf(create_info.applicationInfo.applicationName, sizeof(create_info.applicationInfo.applicationName), "%s", "OpenXR Explorer");
	snprintf(create_info.applicationInfo.engineName,      sizeof(create_info.applicationInfo.engineName     ), "None");
	
	XrResult result = xrCreateInstance(&create_info, &xr_instance);
	if (XR_FAILED(result)) {
		xr_instance_err = openxr_result_string(result);
	}
}

///////////////////////////////////////////

void openxr_init_system(XrFormFactor form) {
	if (xr_instance_err != nullptr) {
		xr_system_err = "XrInstance not available!";
		return;
	}
	if (xr_system_id != XR_NULL_SYSTEM_ID || xr_system_err != nullptr) 
		return;

	XrSystemGetInfo system_info = { XR_TYPE_SYSTEM_GET_INFO };
	system_info.formFactor = form;
	XrResult result = xrGetSystem(xr_instance, &system_info, &xr_system_id);
	if (XR_FAILED(result)) {
		xr_system_err = openxr_result_string(result);
	}
}

///////////////////////////////////////////

void openxr_init_session() {
	if (xr_instance_err != nullptr) {
		xr_session_err = "XrInstance not available!";
		return;
	}
	if (xr_system_err != nullptr) {
		xr_session_err = "XrSystemId not available!";
		return;
	}
	if (xr_session != XR_NULL_HANDLE || xr_session_err != nullptr)
		return;

	skg_platform_data_t platform = skg_get_platform_data();
#if defined(SKG_OPENGL) && defined(__linux__)
	XrGraphicsBindingOpenGLXlibKHR gfx_binding = { XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR };
	gfx_binding.xDisplay    = (Display*  )platform._x_display;
	gfx_binding.visualid    = *(uint32_t *)platform._visual_id;
	gfx_binding.glxFBConfig = (GLXFBConfig)platform._glx_fb_config;
	gfx_binding.glxDrawable = (GLXDrawable)platform._glx_drawable;
	gfx_binding.glxContext  = (GLXContext )platform._glx_context;
#elif defined(SKG_OPENGL) && defined(_WIN32)
	XrGraphicsBindingOpenGLKHR gfx_binding = { XR_TYPE_GRAPHICS_BINDING_OPENGL_KHR };
	gfx_binding.hDC   = (HDC  )platform._gl_hdc;
	gfx_binding.hGLRC = (HGLRC)platform._gl_hrc;
#elif defined(XR_USE_GRAPHICS_API_D3D11)
	PFN_xrGetD3D11GraphicsRequirementsKHR ext_xrGetD3D11GraphicsRequirementsKHR;
	XrGraphicsRequirementsD3D11KHR        requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
	XrGraphicsBindingD3D11KHR             gfx_binding = { XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
	xrGetInstanceProcAddr(xr_instance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction *)(&ext_xrGetD3D11GraphicsRequirementsKHR));
	ext_xrGetD3D11GraphicsRequirementsKHR(xr_instance, xr_system_id, &requirement);
	gfx_binding.device = (ID3D11Device*)platform._d3d11_device;
#endif

	XrSessionCreateInfo session_info = { XR_TYPE_SESSION_CREATE_INFO };
	session_info.next     = &gfx_binding;
	session_info.systemId = xr_system_id;
	XrResult result = xrCreateSession(xr_instance, &session_info, &xr_session);
	if (XR_FAILED(result)) {
		xr_session_err = openxr_result_string(result);
	}
}

///////////////////////////////////////////

xr_extensions_t openxr_load_exts() {
	xr_extensions_t result = {};

	// Load and sort extensions
	uint32_t count = 0;
	if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &count, nullptr)))
		return result;
	result.extensions = array_t<XrExtensionProperties>::make_fill(count, {XR_TYPE_EXTENSION_PROPERTIES});
	xrEnumerateInstanceExtensionProperties(nullptr, count, &count, result.extensions.data);
	result.extensions.sort([](const XrExtensionProperties &a, const XrExtensionProperties &b) {
		return strcmp(a.extensionName, b.extensionName);
	});

	display_table_t table = {};
	table.name_func = "xrEnumerateInstanceExtensionProperties";
	table.name_type = "XrExtensionProperties";
	table.spec      = "extensions";
	table.tag       = display_tag_features;
	table.column_count = 3;
	table.header_row   = true;
	table.cols[0].add({ "Extension Name" });
	table.cols[1].add({ "Version", "Version" });
	table.cols[2].add({ "Spec", "Spec" });
	for (size_t i = 0; i < result.extensions.count; i++) {
		table.cols[0].add({result.extensions[i].extensionName});
		table.cols[1].add({new_string("v%u",result.extensions[i].extensionVersion)});
		table.cols[2].add({nullptr, result.extensions[i].extensionName});
	}
	xr_tables.add(table);

	// Load and sort layers
	count = 0;
	if (XR_FAILED(xrEnumerateApiLayerProperties(0, &count, nullptr)))
		return result;
	result.layers = array_t<XrApiLayerProperties>::make_fill(count, {XR_TYPE_API_LAYER_PROPERTIES});
	xrEnumerateApiLayerProperties(count, &count, result.layers.data);
	result.layers.sort([](const XrApiLayerProperties &a, const XrApiLayerProperties &b) {
		return strcmp(a.layerName, b.layerName);
	});

	table = {};
	table.name_func = "xrEnumerateApiLayerProperties";
	table.name_type = "XrApiLayerProperties";
	table.spec      = "api-layers";
	table.tag       = display_tag_features;
	table.column_count = 3;
	table.header_row   = true;
	table.cols[0].add({ "Layer Name" });
	table.cols[1].add({ "Description" });
	table.cols[2].add({ "Version", "Version" });
	for (size_t i = 0; i < result.layers.count; i++) {
		table.cols[0].add({result.layers[i].layerName});
		table.cols[1].add({result.layers[i].description});
		table.cols[2].add({new_string("v%u",result.layers[i].layerVersion)});
	}
	xr_tables.add(table);

	return result;
}

///////////////////////////////////////////

xr_properties_t openxr_load_properties() {
	xr_properties_t result = {};

	//// Instance properties ////

	display_table_t table = {};
	table.name_func = "xrGetInstanceProperties";
	table.name_type = "XrInstanceProperties";
	table.spec      = "XrInstanceProperties";
	table.tag       = display_tag_properties;
	table.column_count = 2;

	if (!xr_instance_err) {
		result.instance = { XR_TYPE_INSTANCE_PROPERTIES };
		XrResult error = xrGetInstanceProperties(xr_instance, &result.instance);
		if (XR_FAILED(error)) {
			table.error = openxr_result_string(error);
		} else {
			table.cols[0].add({ "runtimeName"    }); table.cols[1].add({ new_string("%s", result.instance.runtimeName) });
			table.cols[0].add({ "runtimeVersion" }); table.cols[1].add({ new_string("%d.%d.%d",
				(int32_t)XR_VERSION_MAJOR(result.instance.runtimeVersion),
				(int32_t)XR_VERSION_MINOR(result.instance.runtimeVersion),
				(int32_t)XR_VERSION_PATCH(result.instance.runtimeVersion)) });
		}
	} else {
		table.error = "No XrInstance available";
	}
	xr_tables.add(table);

	//// System properties ////

	const char *properties_err = nullptr;
	if (!xr_instance_err && !xr_system_err) {
		result.system        = { XR_TYPE_SYSTEM_PROPERTIES };
		result.hand_tracking = { XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
		result.hand_mesh     = { XR_TYPE_SYSTEM_HAND_TRACKING_MESH_PROPERTIES_MSFT };
		result.gaze          = { XR_TYPE_SYSTEM_EYE_GAZE_INTERACTION_PROPERTIES_EXT };

		result.system       .next = &result.hand_tracking;
		result.hand_tracking.next = &result.hand_mesh;
		result.hand_mesh    .next = &result.gaze;

		XrResult error = xrGetSystemProperties(xr_instance, xr_system_id, &result.system);
		if (XR_FAILED(error)) {
			properties_err = openxr_result_string(error);
		}
	} else {
		if (xr_system_err)   properties_err = "No XrSystemId available";
		if (xr_instance_err) properties_err = "No XrInstance available";
	}

	table = {};
	table.name_func = "xrGetSystemProperties";
	table.name_type = "XrSystemProperties";
	table.spec      = "XrSystemProperties";
	table.error     = properties_err;
	table.tag       = display_tag_properties;
	table.column_count = 2;
	table.cols[0].add({"systemName"         }); table.cols[1].add({new_string("%s",result.system.systemName)});
	table.cols[0].add({"vendorId"           }); table.cols[1].add({new_string("%u",result.system.vendorId)});
	table.cols[0].add({"orientationTracking"}); table.cols[1].add({result.system.trackingProperties.orientationTracking ? "True":"False"});
	table.cols[0].add({"positionTracking"   }); table.cols[1].add({result.system.trackingProperties.positionTracking ? "True":"False"});
	table.cols[0].add({"graphics.maxLayerCount"          }); table.cols[1].add({new_string("%u", result.system.graphicsProperties.maxLayerCount)});
	table.cols[0].add({"graphics.maxSwapchainImageWidth" }); table.cols[1].add({new_string("%u", result.system.graphicsProperties.maxSwapchainImageWidth)});
	table.cols[0].add({"graphics.maxSwapchainImageHeight"}); table.cols[1].add({new_string("%u", result.system.graphicsProperties.maxSwapchainImageHeight)});
	xr_tables.add(table);

	table = {};
	table.name_func = "xrGetSystemProperties";
	table.name_type = "XrSystemHandTrackingPropertiesEXT";
	table.spec      = "XrSystemHandTrackingPropertiesEXT";
	table.error     = properties_err;
	table.tag       = display_tag_properties;
	table.show_type = true;
	table.column_count = 2;
	table.cols[0].add({"supportsHandTracking"}); table.cols[1].add({result.hand_tracking.supportsHandTracking ? "True":"False"});
	xr_tables.add(table);

	table = {};
	table.name_func = "xrGetSystemProperties";
	table.name_type = "XrSystemHandTrackingMeshPropertiesMSFT";
	table.spec      = "XrSystemHandTrackingMeshPropertiesMSFT";
	table.error     = properties_err;
	table.tag       = display_tag_properties;
	table.show_type = true;
	table.column_count = 2;
	table.cols[0].add({"supportsHandTrackingMesh"}); table.cols[1].add({result.hand_mesh.supportsHandTrackingMesh ? "True":"False"});
	table.cols[0].add({"maxHandMeshIndexCount"   }); table.cols[1].add({new_string("%u", result.hand_mesh.maxHandMeshIndexCount)});
	table.cols[0].add({"maxHandMeshVertexCount"  }); table.cols[1].add({new_string("%u", result.hand_mesh.maxHandMeshVertexCount)});
	xr_tables.add(table);

	table = {};
	table.name_func = "xrGetSystemProperties";
	table.name_type = "XrSystemEyeGazeInteractionPropertiesEXT";
	table.spec      = "XrSystemEyeGazeInteractionPropertiesEXT";
	table.error     = properties_err;
	table.tag       = display_tag_properties;
	table.show_type = true;
	table.column_count = 2;
	table.cols[0].add({"supportsEyeGazeInteraction"}); table.cols[1].add({result.gaze.supportsEyeGazeInteraction ? "True":"False"});
	xr_tables.add(table);

	return result;
}

///////////////////////////////////////////

xr_view_info_t openxr_load_view(XrViewConfigurationType view_config) {
	xr_view_info_t result = {};

	if (!xr_instance_err && ! xr_system_err) {
		// Get the list of available configurations
		uint32_t count = 0;
		xrEnumerateViewConfigurations(xr_instance, xr_system_id, 0, &count, nullptr);
		result.available_configs = array_t<XrViewConfigurationType>::make_fill(count, (XrViewConfigurationType)0);
		xrEnumerateViewConfigurations(xr_instance, xr_system_id, count, &count, result.available_configs.data);
		result.available_config_names.resize(count);
		for (size_t i = 0; i < count; i++) {
			switch (result.available_configs[i]) {
#define CASE_GET_NAME(e, val) case e: result.available_config_names.add( #e ); break;
				XR_LIST_ENUM_XrViewConfigurationType(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
	}

	// If the caller didn't select a view config, then we'll use the default
	result.current_config = view_config == 0 && result.available_configs.count > 0
		? result.available_configs[0]
		: view_config;

	display_table_t table = {};
	table.name_func = "xrGetViewConfigurationProperties";
	table.name_type = "XrViewConfigurationProperties";
	table.spec      = "XrViewConfigurationProperties";
	table.tag       = display_tag_view;
	table.column_count = 2;

	if (!xr_instance_err && !xr_system_err) {
		result.config_properties = { XR_TYPE_VIEW_CONFIGURATION_PROPERTIES };
		XrResult error = xrGetViewConfigurationProperties(xr_instance, xr_system_id, result.current_config, &result.config_properties);
		if (XR_FAILED(error)) {
			table.error = openxr_result_string(error);
		} else {
			table.cols[0].add({"fovMutable"}); table.cols[1].add({new_string("%s", result.config_properties.fovMutable ? "True" : "False")});
		}
	} else {
		if (xr_system_err)   table.error = "No XrSystemId available";
		if (xr_instance_err) table.error = "No XrInstance available";
	}
	xr_tables.add(table);

	// Load view configuration

	table = {};
	table.name_func = "xrEnumerateViewConfigurationViews";
	table.name_type = "XrViewConfigurationView";
	table.spec      = "XrViewConfigurationView";
	table.tag       = display_tag_view;
	table.column_count = 2;

	if (!xr_instance_err && !xr_system_err) {
		uint32_t count = 0;
		XrResult error = xrEnumerateViewConfigurationViews(xr_instance, xr_system_id, result.current_config, 0, &count, nullptr);
		result.config_views = array_t<XrViewConfigurationView>::make_fill(count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
		xrEnumerateViewConfigurationViews(xr_instance, xr_system_id, result.current_config, count, &count, result.config_views.data);

		if (XR_FAILED(error)) {
			table.error = openxr_result_string(error);
		} else {
			for (uint32_t i = 0; i < result.config_views.count; i++) {
				table.cols[0].add({new_string("View %u", i)         }); table.cols[1].add({""});
				table.cols[0].add({"recommendedImageRectWidth"      }); table.cols[1].add({new_string("%u", result.config_views[i].recommendedImageRectWidth)});
				table.cols[0].add({"recommendedImageRectHeight"     }); table.cols[1].add({new_string("%u", result.config_views[i].recommendedImageRectHeight)});
				table.cols[0].add({"recommendedSwapchainSampleCount"}); table.cols[1].add({new_string("%u", result.config_views[i].recommendedSwapchainSampleCount)});
				table.cols[0].add({"maxImageRectWidth"              }); table.cols[1].add({new_string("%u", result.config_views[i].maxImageRectWidth)});
				table.cols[0].add({"maxImageRectHeight"             }); table.cols[1].add({new_string("%u", result.config_views[i].maxImageRectHeight)});
				table.cols[0].add({"maxSwapchainSampleCount"        }); table.cols[1].add({new_string("%u", result.config_views[i].maxSwapchainSampleCount)});
			}
		}
	} else {
		if (xr_system_err)   table.error = "No XrSystemId available";
		if (xr_instance_err) table.error = "No XrInstance available";
	}
	xr_tables.add(table);
	return result;
}

///////////////////////////////////////////

void openxr_load_enums(xr_settings_t settings) {
	for (size_t i = 0; i < xr_misc_enums.count; i++) {
		xr_misc_enums[i].items.clear();
		if (!xr_misc_enums[i].requires_session || settings.allow_session) {
			if (xr_misc_enums[i].requires_session) 
				openxr_init_session();
			XrResult error = xr_misc_enums[i].load_info(&xr_misc_enums[i], settings);

			display_table_t table = {};
			table.name_func = xr_misc_enums[i].source_fn_name;
			table.name_type = xr_misc_enums[i].source_type_name;
			table.spec      = xr_misc_enums[i].spec_link;
			table.tag       = xr_misc_enums[i].tag;
			table.column_count = 1;
			for (size_t e = 0; e < xr_misc_enums[i].items.count; e++) {
				table.cols[0].add({ xr_misc_enums[i].items[e] });
			}
			if (XR_FAILED(error)) {
				table.error = openxr_result_string(error);
			}
			xr_tables.add(table);
		} else {
			xr_misc_enums[i].items.add("XrSession unavailable!");
		}
	}
}

///////////////////////////////////////////

void openxr_register_enums() {
	xr_misc_enums.clear();

	xr_enum_info_t info = { "xrEnumerateReferenceSpaces" };
	info.source_type_name = "XrReferenceSpaceType";
	info.spec_link        = "reference-spaces";
	info.requires_session = true;
	info.tag              = display_tag_misc;
	info.load_info        = [](xr_enum_info_t *ref_info, xr_settings_t settings) {
		uint32_t count = 0;
		XrResult error = xrEnumerateReferenceSpaces(xr_session, 0, &count, nullptr);
		array_t<XrReferenceSpaceType> items(count, (XrReferenceSpaceType)0);
		xrEnumerateReferenceSpaces(xr_session, count, &count, items.data);
		for (size_t i = 0; i < items.count; i++) {
			switch (items[i]) {
#define CASE_GET_NAME(e, val) case e: ref_info->items.add( #e ); break;
				XR_LIST_ENUM_XrReferenceSpaceType(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
		items.free();
		return error;
	};
	xr_misc_enums.add(info);

	
	info = { "xrEnumerateEnvironmentBlendModes" };
	info.source_type_name = "XrEnvironmentBlendMode";
	info.spec_link        = "XrEnvironmentBlendMode";
	info.requires_session = false;
	info.tag              = display_tag_view;
	info.load_info        = [](xr_enum_info_t *ref_info, xr_settings_t settings) {
		if (settings.view_config == 0) {
			if (xr_view.available_configs.count > 0) {
				settings.view_config = xr_view.available_configs[0];
			}
		}

		uint32_t count = 0;
		XrResult error = xrEnumerateEnvironmentBlendModes(xr_instance, xr_system_id, settings.view_config, 0, &count, nullptr);
		array_t<XrEnvironmentBlendMode> items(count, (XrEnvironmentBlendMode)0);
		xrEnumerateEnvironmentBlendModes(xr_instance, xr_system_id, settings.view_config, count, &count, items.data);

		for (size_t i = 0; i < count; i++) {
			switch (items[i]) {
#define CASE_GET_NAME(e, val) case e: ref_info->items.add({ #e }); break;
				XR_LIST_ENUM_XrEnvironmentBlendMode(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
		items.free();
		return error;
	};
	xr_misc_enums.add(info);

	info = { "xrEnumerateSwapchainFormats" };
	info.source_type_name = "skg_tex_fmt_";
	info.spec_link        = "xrEnumerateSwapchainFormats";
	info.requires_session = true;
	info.tag              = display_tag_view;
	info.load_info        = [](xr_enum_info_t *ref_info, xr_settings_t settings) {
		uint32_t count = 0;
		XrResult error = xrEnumerateSwapchainFormats(xr_session, 0, &count, nullptr);
		array_t<int64_t> formats(count, 0);
		xrEnumerateSwapchainFormats(xr_session, count, &count, formats.data);

		for (size_t i = 0; i < formats.count; i++) {
			skg_tex_fmt_ format = skg_tex_fmt_from_native(formats[i]);
			switch (format) {
			case skg_tex_fmt_none:          ref_info->items.add("skg_tex_fmt_unrecognized");    break;
			case skg_tex_fmt_rgba32:        ref_info->items.add("skg_tex_fmt_rgba32");          break;
			case skg_tex_fmt_rgba32_linear: ref_info->items.add("skg_tex_fmt_rgba32_linear");   break;
			case skg_tex_fmt_bgra32:        ref_info->items.add("skg_tex_fmt_bgra32");          break;
			case skg_tex_fmt_bgra32_linear: ref_info->items.add("skg_tex_fmt_bgra32_linear");   break;
			case skg_tex_fmt_rg11b10:       ref_info->items.add("skg_tex_fmt_rg11b10");         break;
			case skg_tex_fmt_rgb10a2:       ref_info->items.add("skg_tex_fmt_rgb10a2");         break;
			case skg_tex_fmt_rgba64u:       ref_info->items.add("skg_tex_fmt_rgba64u");         break;
			case skg_tex_fmt_rgba64s:       ref_info->items.add("skg_tex_fmt_rgba64s");         break;
			case skg_tex_fmt_rgba64f:       ref_info->items.add("skg_tex_fmt_rgba64f");         break;
			case skg_tex_fmt_rgba128:       ref_info->items.add("skg_tex_fmt_rgba128");         break;
			case skg_tex_fmt_r8:            ref_info->items.add("skg_tex_fmt_r8");              break;
			case skg_tex_fmt_r16:           ref_info->items.add("skg_tex_fmt_r16");             break;
			case skg_tex_fmt_r32:           ref_info->items.add("skg_tex_fmt_r32");             break;
			case skg_tex_fmt_depthstencil:  ref_info->items.add("skg_tex_fmt_depth24stencil8"); break;
			case skg_tex_fmt_depth32:       ref_info->items.add("skg_tex_fmt_depth32");         break;
			case skg_tex_fmt_depth16:       ref_info->items.add("skg_tex_fmt_depth16");         break;
			}
		}
		formats.free();
		return error;
	};
	xr_misc_enums.add(info);
}
#include "openxr_info.h"
#include "openxr_properties.h"
#include "openxr_properties.h"

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

const char* xr_runtime_name = "No runtime set";

#define XR_NEXT_INSERT(obj, obj_next) obj_next.next = obj.next; obj.next = &obj_next;

/*** Signatures **************************/

bool      openxr_init_instance(array_t<XrExtensionProperties> extensions, XrInstance* instance, const char** instance_err);
bool      openxr_init_system  (XrInstance instance, XrFormFactor form, XrSystemId* system_id, const char** system_err);
XrSession openxr_init_session (XrInstance instance, XrSystemId system_id, const char** session_err);

xr_extensions_t         openxr_load_exts      (array_t<display_table_t>* tables);
xr_properties_t         openxr_load_properties(XrInstance instance, XrSystemId system_id, const char** runtime_name, array_t<display_table_t>* tables);
xr_view_info_t          openxr_load_view      (XrInstance instance, XrSystemId system_id, XrViewConfigurationType view_config, array_t<display_table_t>* tables);
void                    openxr_load_enums     (XrInstance instance, XrSystemId system_id, xr_settings_t settings, array_t<xr_enum_info_t> misc_enums, array_t<display_table_t>* tables, const char** session_err);
const char *            openxr_result_string  (XrResult result);
array_t<xr_enum_info_t> openxr_register_enums ();
bool                    openxr_has_ext        (const char *ext_name);


/*** Code ********************************/

void openxr_info_reload(xr_settings_t settings, ft_mutex_t* lock) {
	if (lock) ft_mutex_lock(*lock);
	openxr_info_release();
	if (lock) ft_mutex_unlock(*lock);

	array_t<display_table_t> tables = {};
	xr_extensions_t new_extensions = openxr_load_exts(&tables);

	if (lock) ft_mutex_lock(*lock);
	xr_extensions = new_extensions;
	if (lock) ft_mutex_unlock(*lock);

	if(!openxr_init_instance(xr_extensions.extensions, &xr_instance, &xr_instance_err)){
		xr_system_err  = "No XrInstance available";
	}
	if (!openxr_init_system(xr_instance, settings.form, &xr_system_id, &xr_system_err)) {
		if (settings.allow_session)
			xr_session_err = "No XrSystemId available";
	}
	xr_properties = openxr_load_properties(xr_instance, xr_system_id, &xr_runtime_name, &tables);
	xr_view       = openxr_load_view      (xr_instance, xr_system_id, settings.view_config, &tables);

	xr_misc_enums = openxr_register_enums();
	openxr_load_enums(xr_instance, xr_system_id, settings, xr_misc_enums, &tables, &xr_session_err);

	if (lock) ft_mutex_lock(*lock);
	xr_tables = tables;
	if (lock) ft_mutex_unlock(*lock);
}

///////////////////////////////////////////

void openxr_info_release() {
	xr_misc_enums.each([](xr_enum_info_t &i) { i.items.free(); });
	xr_misc_enums.clear();
	xr_properties = {};
	xr_view.available_configs     .clear();
	xr_view.available_config_names.clear();
	xr_view.config_views          .clear();
	xr_view = {};
	xr_extensions.extensions.clear();
	xr_extensions.layers    .clear();
	xr_extensions = {};
	xr_runtime_name = "No runtime set";

	xr_table_strings.each(free);
	xr_table_strings.clear();
	xr_tables.each([](display_table_t &t) {for (int32_t i=0; i<t.column_count; i++) t.cols[i].free(); });
	xr_tables.clear();

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

const char* openxr_path_string(XrPath path) {
	uint32_t count  = 0;
	XrResult result = xrPathToString(xr_instance, path, 0, &count, nullptr);
	if (XR_FAILED(result)) return openxr_result_string(result);

	char* path_str = (char*)malloc(count + 1);
	result = xrPathToString(xr_instance, path, count, &count, path_str);
	if (XR_FAILED(result)) return openxr_result_string(result);

	return path_str;
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

bool openxr_init_instance(array_t<XrExtensionProperties> extensions, XrInstance* instance, const char **instance_err) {
	if (*instance != XR_NULL_HANDLE || *instance_err != nullptr)
		return false;

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
	create_info.enabledExtensionCount = (uint32_t)exts.count;
	create_info.enabledExtensionNames = exts.data;
	create_info.enabledApiLayerCount  = 0;
	create_info.enabledApiLayerNames  = nullptr;
	create_info.applicationInfo.applicationVersion = 1;
	create_info.applicationInfo.engineVersion      = 1;
	create_info.applicationInfo.apiVersion         = XR_CURRENT_API_VERSION;
	snprintf(create_info.applicationInfo.applicationName, sizeof(create_info.applicationInfo.applicationName), "%s", "OpenXR Explorer");
	snprintf(create_info.applicationInfo.engineName,      sizeof(create_info.applicationInfo.engineName     ), "None");
	
	XrResult result = xrCreateInstance(&create_info, instance);
	if (result == XR_ERROR_API_VERSION_UNSUPPORTED) {
		create_info.applicationInfo.apiVersion = XR_API_VERSION_1_0;
		result = xrCreateInstance(&create_info, instance);
	}
	if (XR_FAILED(result)) {
		*instance_err = openxr_result_string(result);
		return false;
	}
	return true;
}

///////////////////////////////////////////

bool openxr_init_system(XrInstance instance, XrFormFactor form, XrSystemId* system_id, const char** system_err) {
	if (instance == XR_NULL_HANDLE || *system_id != XR_NULL_SYSTEM_ID || *system_err != nullptr) 
		return false;

	XrSystemGetInfo system_info = { XR_TYPE_SYSTEM_GET_INFO };
	system_info.formFactor = form;
	XrResult result = xrGetSystem(instance, &system_info, system_id);
	if (XR_FAILED(result)) {
		*system_err = openxr_result_string(result);
		return false;
	}
	return true;
}

///////////////////////////////////////////

XrSession openxr_init_session(XrInstance instance, XrSystemId system_id, const char** session_err) {
	if (instance == XR_NULL_HANDLE) {
		*session_err = "No XrInstance";
		return XR_NULL_HANDLE;
	}
	if (system_id == XR_NULL_SYSTEM_ID) {
		*session_err = "No XrSystemId";
		return XR_NULL_HANDLE;
	}
	if (*session_err != nullptr)
		return XR_NULL_HANDLE;

	skg_platform_data_t platform = skg_get_platform_data();
#if defined(SKG_OPENGL) && defined(__linux__)
	PFN_xrGetOpenGLGraphicsRequirementsKHR ext_xrGetOpenGLGraphicsRequirementsKHR;
	XrGraphicsRequirementsOpenGLKHR        requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
	XrGraphicsBindingOpenGLXlibKHR         gfx_binding = { XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR };
	gfx_binding.xDisplay    = (Display*  )platform._x_display;
	gfx_binding.visualid    = *(uint32_t *)platform._visual_id;
	gfx_binding.glxFBConfig = (GLXFBConfig)platform._glx_fb_config;
	gfx_binding.glxDrawable = (GLXDrawable)platform._glx_drawable;
	gfx_binding.glxContext  = (GLXContext )platform._glx_context;
	xrGetInstanceProcAddr(instance, "xrGetOpenGLGraphicsRequirementsKHR", (PFN_xrVoidFunction *)(&ext_xrGetOpenGLGraphicsRequirementsKHR));
	ext_xrGetOpenGLGraphicsRequirementsKHR(instance, system_id, &requirement);
#elif defined(SKG_OPENGL) && defined(_WIN32)
	XrGraphicsBindingOpenGLKHR gfx_binding = { XR_TYPE_GRAPHICS_BINDING_OPENGL_KHR };
	gfx_binding.hDC   = (HDC  )platform._gl_hdc;
	gfx_binding.hGLRC = (HGLRC)platform._gl_hrc;
#elif defined(XR_USE_GRAPHICS_API_D3D11)
	PFN_xrGetD3D11GraphicsRequirementsKHR ext_xrGetD3D11GraphicsRequirementsKHR;
	XrGraphicsRequirementsD3D11KHR        requirement = { XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
	XrGraphicsBindingD3D11KHR             gfx_binding = { XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
	xrGetInstanceProcAddr(xr_instance, "xrGetD3D11GraphicsRequirementsKHR", (PFN_xrVoidFunction *)(&ext_xrGetD3D11GraphicsRequirementsKHR));
	ext_xrGetD3D11GraphicsRequirementsKHR(instance, system_id, &requirement);
	gfx_binding.device = (ID3D11Device*)platform._d3d11_device;
#endif

	XrSessionCreateInfo session_info = { XR_TYPE_SESSION_CREATE_INFO };
	session_info.next     = &gfx_binding;
	session_info.systemId = system_id;

	// If the headless extension is present, we don't need a graphics binding!
	if (openxr_has_ext("XR_MND_headless"))
		session_info.next = nullptr;

	XrSession session;
	XrResult  result = xrCreateSession(instance, &session_info, &session);
	if (XR_FAILED(result)) {
		*session_err = openxr_result_string(result);
		return XR_NULL_HANDLE;
	}
	return session;
}

///////////////////////////////////////////

xr_extensions_t openxr_load_exts(array_t<display_table_t>* tables) {
	xr_extensions_t result = {};

	// Load layers.
	//
	// Layers are not sorted because the order is important; for example, layers that
	// use an extension should be before layers that provide the extension.
	uint32_t count = 0;
	if (XR_FAILED(xrEnumerateApiLayerProperties(0, &count, nullptr)))
		return result;
	result.layers = array_t<XrApiLayerProperties>::make_fill(count, {XR_TYPE_API_LAYER_PROPERTIES});
	xrEnumerateApiLayerProperties(count, &count, result.layers.data);

	display_table_t table = {};
	table.name_func = "xrEnumerateApiLayerProperties";
	table.name_type = "XrApiLayerProperties";
	table.spec      = "api-layers";
	table.tag       = display_tag_features;
	table.column_count = 3;
	table.header_row   = true;
	if (result.layers.count > 0) {
		table.cols[0].add({ "Layer Name" });
		table.cols[1].add({ "Description" });
		table.cols[2].add({ "Version", "Version" });
		for (size_t i = 0; i < result.layers.count; i++) {
			table.cols[0].add({result.layers[i].layerName});
			table.cols[1].add({result.layers[i].description});
			table.cols[2].add({new_string("v%u",result.layers[i].layerVersion)});
		}
	} else {
		table.error = "No layers present";
	}
	tables->add(table);

	// Load and sort extensions
	count = 0;
	if (XR_FAILED(xrEnumerateInstanceExtensionProperties(nullptr, 0, &count, nullptr)))
		return result;
	result.extensions = array_t<XrExtensionProperties>::make_fill(count, {XR_TYPE_EXTENSION_PROPERTIES});
	xrEnumerateInstanceExtensionProperties(nullptr, count, &count, result.extensions.data);
	result.extensions.sort([](const XrExtensionProperties &a, const XrExtensionProperties &b) {
		return strcmp(a.extensionName, b.extensionName);
	});

	table = {};
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
	tables->add(table);

	return result;
}

///////////////////////////////////////////

bool openxr_has_ext(const char *ext_name){
	for (int32_t i = 0; i < xr_extensions.extensions.count ; i++) {
		if (strcmp(ext_name, xr_extensions.extensions[i].extensionName) == 0)
		return true;
	}
	return false;
}

///////////////////////////////////////////

xr_properties_t openxr_load_properties(XrInstance instance, XrSystemId system_id, const char** runtime_name, array_t<display_table_t>* tables) {
	xr_properties_t result = {};

	//// Instance properties ////

	display_table_t table = {};
	table.name_func = "xrGetInstanceProperties";
	table.name_type = "XrInstanceProperties";
	table.spec      = "XrInstanceProperties";
	table.tag       = display_tag_properties;
	table.column_count = 2;

	if (instance != XR_NULL_HANDLE) {
		result.instance = { XR_TYPE_INSTANCE_PROPERTIES };
		XrResult error = xrGetInstanceProperties(instance, &result.instance);
		if (XR_FAILED(error)) {
			table.error = openxr_result_string(error);
		} else {
			*runtime_name = new_string("%s", result.instance.runtimeName);
			table.cols[0].add({ "runtimeName"    }); table.cols[1].add({ new_string("%s", result.instance.runtimeName) });
			table.cols[0].add({ "runtimeVersion" }); table.cols[1].add({ new_string("%d.%d.%d",
				(int32_t)XR_VERSION_MAJOR(result.instance.runtimeVersion),
				(int32_t)XR_VERSION_MINOR(result.instance.runtimeVersion),
				(int32_t)XR_VERSION_PATCH(result.instance.runtimeVersion)) });
		}
	} else {
		table.error = "No XrInstance available";
	}
	tables->add(table);

	//// System properties ////
	
	openxr_load_system_properties(instance, system_id, tables);

	return result;
}

///////////////////////////////////////////

xr_view_info_t openxr_load_view(XrInstance instance, XrSystemId system_id, XrViewConfigurationType view_config, array_t<display_table_t>* tables) {
	xr_view_info_t result = {};

	if (instance != XR_NULL_HANDLE && system_id != XR_NULL_SYSTEM_ID) {
		// Get the list of available configurations
		uint32_t count = 0;
		xrEnumerateViewConfigurations(instance, system_id, 0, &count, nullptr);
		result.available_configs = array_t<XrViewConfigurationType>::make_fill(count, (XrViewConfigurationType)0);
		xrEnumerateViewConfigurations(instance, system_id, count, &count, result.available_configs.data);
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

	result.config_properties = { XR_TYPE_VIEW_CONFIGURATION_PROPERTIES };
	XrResult error = xrGetViewConfigurationProperties(instance, system_id, result.current_config, &result.config_properties);
	if (XR_FAILED(error)) {
		table.error = openxr_result_string(error);
	} else {
		table.cols[0].add({"fovMutable"}); table.cols[1].add({new_string("%s", result.config_properties.fovMutable ? "True" : "False")});
	}
	tables->add(table);

	// Load view configuration

	table = {};
	table.name_func = "xrEnumerateViewConfigurationViews";
	table.name_type = "XrViewConfigurationView";
	table.spec      = "XrViewConfigurationView";
	table.tag       = display_tag_view;
	table.column_count = 2;

	uint32_t count = 0;
	error = xrEnumerateViewConfigurationViews(instance, system_id, result.current_config, 0, &count, nullptr);
	result.config_views = array_t<XrViewConfigurationView>::make_fill(count, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
	xrEnumerateViewConfigurationViews(instance, system_id, result.current_config, count, &count, result.config_views.data);

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
	tables->add(table);
	return result;
}

///////////////////////////////////////////

void openxr_load_enums(XrInstance instance, XrSystemId system_id, xr_settings_t settings, array_t<xr_enum_info_t> misc_enums, array_t<display_table_t>* tables, const char** session_err) {
	// Check if any of the enums need a session
	XrSession session = XR_NULL_HANDLE;
	bool requires_session = false;
	for (size_t i = 0; i < misc_enums.count; i++) {
		if (misc_enums[i].requires_session) {
			requires_session = true;
			break;
		}
	}

	if (requires_session && settings.allow_session == true) {
		session = openxr_init_session(instance, system_id, &xr_session_err);
	}
	if (requires_session && settings.allow_session == false) {
		*session_err = "Enable 'Create XrSession'";
	}

	for (size_t i = 0; i < misc_enums.count; i++) {
		xr_enum_info_t *e = &misc_enums[i];
		e->items.clear();

		display_table_t table = {};
		table.name_func = e->source_fn_name;
		table.name_type = e->source_type_name;
		table.spec      = e->spec_link;
		table.tag       = e->tag;
		table.column_count = 1;

		if ((!e->requires_session  || session   != XR_NULL_HANDLE) &&
			(!e->requires_instance || instance  != XR_NULL_HANDLE) &&
			(!e->requires_system   || system_id != XR_NULL_SYSTEM_ID)) {

			XrResult error = e->load_info(instance, system_id, session, e, settings);

			for (size_t t = 0; t < e->items.count; t++) {
				table.cols[0].add({ e->items[t] });
			}
			if (XR_FAILED(error)) {
				table.error = openxr_result_string(error);
			}
		} else {
			if      (e->requires_instance && instance  == XR_NULL_HANDLE   ) table.error = "No XrInstance available";
			else if (e->requires_system   && system_id == XR_NULL_SYSTEM_ID) table.error = "No XrSystemId available";
			else if (e->requires_session  && session   == XR_NULL_HANDLE   ) table.error = "No XrSession available";
		}

		tables->add(table);
	}
	if (session != XR_NULL_HANDLE)
		xrDestroySession(session);
}

///////////////////////////////////////////

array_t<xr_enum_info_t> openxr_register_enums() {
	array_t<xr_enum_info_t> misc_enums = {};

	xr_enum_info_t info = { "xrEnumerateReferenceSpaces" };
	info.source_type_name = "XrReferenceSpaceType";
	info.spec_link        = "reference-spaces";
	info.requires_session = true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		uint32_t count = 0;
		XrResult error = xrEnumerateReferenceSpaces(session, 0, &count, nullptr);
		array_t<XrReferenceSpaceType> items(count, (XrReferenceSpaceType)0);
		xrEnumerateReferenceSpaces(session, count, &count, items.data);
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
	misc_enums.add(info);

	info = { "xrEnumerateEnvironmentBlendModes" };
	info.source_type_name = "XrEnvironmentBlendMode";
	info.spec_link        = "XrEnvironmentBlendMode";
	info.requires_instance= true;
	info.requires_system  = true;
	info.tag              = display_tag_view;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		if (settings.view_config == 0) {
			if (xr_view.available_configs.count > 0) {
				settings.view_config = xr_view.available_configs[0];
			}
		}

		uint32_t count = 0;
		XrResult error = xrEnumerateEnvironmentBlendModes(instance, system_id, settings.view_config, 0, &count, nullptr);
		array_t<XrEnvironmentBlendMode> items(count, (XrEnvironmentBlendMode)0);
		xrEnumerateEnvironmentBlendModes(instance, system_id, settings.view_config, count, &count, items.data);

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
	misc_enums.add(info);

	info = { "xrEnumerateSwapchainFormats" };
	info.source_type_name = "skg_tex_fmt_";
	info.spec_link        = "xrEnumerateSwapchainFormats";
	info.requires_session = true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		uint32_t count = 0;
		XrResult error = xrEnumerateSwapchainFormats(session, 0, &count, nullptr);
		array_t<int64_t> formats(count, 0);
		xrEnumerateSwapchainFormats(session, count, &count, formats.data);

		for (size_t i = 0; i < formats.count; i++) {
			skg_tex_fmt_ format = skg_tex_fmt_from_native(formats[i]);
			switch (format) {
			case skg_tex_fmt_none:          ref_info->items.add(new_string("Unknown 0x%x #%d", formats[i], formats[i])); break;
			case skg_tex_fmt_rgba32:        ref_info->items.add("rgba32");          break;
			case skg_tex_fmt_rgba32_linear: ref_info->items.add("rgba32 linear");   break;
			case skg_tex_fmt_bgra32:        ref_info->items.add("bgra32");          break;
			case skg_tex_fmt_bgra32_linear: ref_info->items.add("bgra32 linear");   break;
			case skg_tex_fmt_rg11b10:       ref_info->items.add("rg11 b10");        break;
			case skg_tex_fmt_rgb10a2:       ref_info->items.add("rgb10 a2");        break;
			case skg_tex_fmt_rgba64u:       ref_info->items.add("rgba64u");         break;
			case skg_tex_fmt_rgba64s:       ref_info->items.add("rgba64s");         break;
			case skg_tex_fmt_rgba64f:       ref_info->items.add("rgba64f");         break;
			case skg_tex_fmt_rgba128:       ref_info->items.add("rgba128");         break;
			case skg_tex_fmt_r8:            ref_info->items.add("r8");              break;
			case skg_tex_fmt_r16:           ref_info->items.add("r16");             break;
			case skg_tex_fmt_r32:           ref_info->items.add("r32");             break;
			case skg_tex_fmt_depthstencil:  ref_info->items.add("depth24 stencil8");break;
			case skg_tex_fmt_depth32:       ref_info->items.add("depth32");         break;
			case skg_tex_fmt_depth16:       ref_info->items.add("depth16");         break;
			}
		}
		formats.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateColorSpacesFB" };
	info.source_type_name = "XrColorSpaceFB";
	info.spec_link        = "XrColorSpaceFB";
	info.requires_session = true;
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateColorSpacesFB xrEnumerateColorSpacesFB;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumerateColorSpacesFB", (PFN_xrVoidFunction *)(&xrEnumerateColorSpacesFB));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateColorSpacesFB(session, 0, &count, nullptr);
		array_t<XrColorSpaceFB> color_spaces(count, (XrColorSpaceFB)0);
		xrEnumerateColorSpacesFB(session, count, &count, color_spaces.data);

		for (size_t i = 0; i < color_spaces.count; i++) {
			switch (color_spaces[i]) {
#define CASE_GET_NAME(e, val) case e: ref_info->items.add({ #e }); break;
				XR_LIST_ENUM_XrColorSpaceFB(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
		color_spaces.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateDisplayRefreshRatesFB" };
	info.source_type_name = "float";
	info.spec_link        = "xrEnumerateDisplayRefreshRatesFB";
	info.requires_session = true;
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateDisplayRefreshRatesFB xrEnumerateDisplayRefreshRatesFB;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumerateDisplayRefreshRatesFB", (PFN_xrVoidFunction *)(&xrEnumerateDisplayRefreshRatesFB));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateDisplayRefreshRatesFB(session, 0, &count, nullptr);
		array_t<float> refresh_rates(count, 0);
		xrEnumerateDisplayRefreshRatesFB(session, count, &count, refresh_rates.data);

		for (size_t i = 0; i < refresh_rates.count; i++) {
			ref_info->items.add({ new_string("%f", refresh_rates[i])});
		}
		refresh_rates.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateRenderModelPathsFB" };
	info.source_type_name = "XrRenderModelPathInfoFB";
	info.spec_link        = "XrRenderModelPathInfoFB";
	info.requires_session = true;
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateRenderModelPathsFB xrEnumerateRenderModelPathsFB;
		XrResult error = xrGetInstanceProcAddr(xr_instance, "xrEnumerateRenderModelPathsFB", (PFN_xrVoidFunction *)(&xrEnumerateRenderModelPathsFB));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateRenderModelPathsFB(session, 0, &count, nullptr);
		array_t<XrRenderModelPathInfoFB> model_paths(count, XrRenderModelPathInfoFB{ XR_TYPE_RENDER_MODEL_PATH_INFO_FB });
		xrEnumerateRenderModelPathsFB(session, count, &count, model_paths.data);

		for (size_t i = 0; i < model_paths.count; i++) {
			ref_info->items.add({ openxr_path_string(model_paths[i].path) });
		}
		model_paths.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateViveTrackerPathsHTCX" };
	info.source_type_name = "XrViveTrackerPathsHTCX";
	info.spec_link        = "XrViveTrackerPathsHTCX";
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateViveTrackerPathsHTCX xrEnumerateViveTrackerPathsHTCX;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumerateViveTrackerPathsHTCX", (PFN_xrVoidFunction *)(&xrEnumerateViveTrackerPathsHTCX));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateViveTrackerPathsHTCX(instance, 0, &count, nullptr);
		array_t<XrViveTrackerPathsHTCX> tracker_paths(count, XrViveTrackerPathsHTCX{ XR_TYPE_VIVE_TRACKER_PATHS_HTCX });
		xrEnumerateViveTrackerPathsHTCX(instance, count, &count, tracker_paths.data);

		// TODO: This needs labels for persistentPath and rolePath, but the current
		// structure doens't exactly allow for this.
		for (size_t i = 0; i < tracker_paths.count; i++) {
			ref_info->items.add({ openxr_path_string(tracker_paths[i].persistentPath) });
			ref_info->items.add({ openxr_path_string(tracker_paths[i].rolePath) });
		}
		tracker_paths.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumeratePerformanceMetricsCounterPathsMETA" };
	info.source_type_name = "XrPath";
	info.spec_link        = "xrEnumeratePerformanceMetricsCounterPathsMETA";
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumeratePerformanceMetricsCounterPathsMETA xrEnumeratePerformanceMetricsCounterPathsMETA;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumeratePerformanceMetricsCounterPathsMETA", (PFN_xrVoidFunction *)(&xrEnumeratePerformanceMetricsCounterPathsMETA));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumeratePerformanceMetricsCounterPathsMETA(instance, 0, &count, nullptr);
		array_t<XrPath> metric_paths(count, {});
		xrEnumeratePerformanceMetricsCounterPathsMETA(instance, count, &count, metric_paths.data);

		for (size_t i = 0; i < metric_paths.count; i++) {
			ref_info->items.add({ openxr_path_string(metric_paths[i]) });
		}
		metric_paths.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateReprojectionModesMSFT" };
	info.source_type_name = "XrReprojectionModeMSFT";
	info.spec_link        = "XrReprojectionModeMSFT";
	info.requires_system  = true;
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateReprojectionModesMSFT xrEnumerateReprojectionModesMSFT;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumerateReprojectionModesMSFT", (PFN_xrVoidFunction *)(&xrEnumerateReprojectionModesMSFT));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateReprojectionModesMSFT(instance, system_id, xr_view.current_config, 0, &count, nullptr);
		array_t<XrReprojectionModeMSFT> reprojection_modes(count, (XrReprojectionModeMSFT)0);
		xrEnumerateReprojectionModesMSFT(instance, system_id, xr_view.current_config, count, &count, reprojection_modes.data);

		for (size_t i = 0; i < reprojection_modes.count; i++) {
			switch (reprojection_modes[i]) {
#define CASE_GET_NAME(e, val) case e: ref_info->items.add({ #e }); break;
				XR_LIST_ENUM_XrReprojectionModeMSFT(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
		reprojection_modes.free();
		return error;
	};
	misc_enums.add(info);

	info = { "xrEnumerateSceneComputeFeaturesMSFT" };
	info.source_type_name = "XrSceneComputeFeatureMSFT";
	info.spec_link        = "XrSceneComputeFeatureMSFT";
	info.requires_system  = true;
	info.requires_instance= true;
	info.tag              = display_tag_misc;
	info.load_info        = [](XrInstance instance, XrSystemId system_id, XrSession session, xr_enum_info_t *ref_info, xr_settings_t settings) {
		PFN_xrEnumerateSceneComputeFeaturesMSFT xrEnumerateSceneComputeFeaturesMSFT;
		XrResult error = xrGetInstanceProcAddr(instance, "xrEnumerateSceneComputeFeaturesMSFT", (PFN_xrVoidFunction *)(&xrEnumerateSceneComputeFeaturesMSFT));
		if (XR_FAILED(error)) return error;

		uint32_t count = 0;
		error = xrEnumerateSceneComputeFeaturesMSFT(instance, system_id, 0, &count, nullptr);
		array_t<XrSceneComputeFeatureMSFT> compute_features(count, (XrSceneComputeFeatureMSFT)0);
		xrEnumerateSceneComputeFeaturesMSFT(instance, system_id, count, &count, compute_features.data);

		for (size_t i = 0; i < compute_features.count; i++) {
			switch (compute_features[i]) {
#define CASE_GET_NAME(e, val) case e: ref_info->items.add({ #e }); break;
				XR_LIST_ENUM_XrSceneComputeFeatureMSFT(CASE_GET_NAME)
#undef CASE_GET_NAME
			}
		}
		compute_features.free();
		return error;
	};
	misc_enums.add(info);

	return misc_enums;
}
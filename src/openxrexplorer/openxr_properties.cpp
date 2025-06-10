// THIS FILE CONTAINS GENERATED CODE!
// Building the project will regenerate this section of code, **be cautious**
// when editing!

#include "openxr_info.h"
#include "openxr_properties.h"

void openxr_load_system_properties(XrInstance instance, XrSystemId system_id, array_t<display_table_t>* tables) {
	const char*     properties_err = nullptr;
	display_table_t table          = {};
	XrResult        error;

	// The general system properties are handwritten, because they are not
	// picked up by the generator!
	XrSystemProperties sys_props = { XR_TYPE_SYSTEM_PROPERTIES };
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemProperties";
	table.spec         = "XrSystemProperties";
	table.column_count = 2;
	table.cols[0].add({"systemName"         }); table.cols[1].add({new_string("%s",sys_props.systemName)});
	table.cols[0].add({"vendorId"           }); table.cols[1].add({new_string("%u",sys_props.vendorId)});
	table.cols[0].add({"orientationTracking"}); table.cols[1].add({sys_props.trackingProperties.orientationTracking ? "True":"False"});
	table.cols[0].add({"positionTracking"   }); table.cols[1].add({sys_props.trackingProperties.positionTracking ? "True":"False"});
	table.cols[0].add({"graphics.maxLayerCount"          }); table.cols[1].add({new_string("%u", sys_props.graphicsProperties.maxLayerCount)});
	table.cols[0].add({"graphics.maxSwapchainImageWidth" }); table.cols[1].add({new_string("%u", sys_props.graphicsProperties.maxSwapchainImageWidth)});
	table.cols[0].add({"graphics.maxSwapchainImageHeight"}); table.cols[1].add({new_string("%u", sys_props.graphicsProperties.maxSwapchainImageHeight)});
	tables->add(table);

// <<GENERATED_CODE_START>>
	// XR_HTC_anchor
	XrSystemAnchorPropertiesHTC props0 = { XR_TYPE_SYSTEM_ANCHOR_PROPERTIES_HTC };
	sys_props.next = &props0;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemAnchorPropertiesHTC";
	table.spec         = "XrSystemAnchorPropertiesHTC";
	table.cols[0].add({"supportsAnchor"});
	table.cols[1].add({props0.supportsAnchor ? "True":"False"});
	tables->add(table);

	// XR_BD_body_tracking
	XrSystemBodyTrackingPropertiesBD props1 = { XR_TYPE_SYSTEM_BODY_TRACKING_PROPERTIES_BD };
	sys_props.next = &props1;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemBodyTrackingPropertiesBD";
	table.spec         = "XrSystemBodyTrackingPropertiesBD";
	table.cols[0].add({"supportsBodyTracking"});
	table.cols[1].add({props1.supportsBodyTracking ? "True":"False"});
	tables->add(table);

	// XR_FB_body_tracking
	XrSystemBodyTrackingPropertiesFB props2 = { XR_TYPE_SYSTEM_BODY_TRACKING_PROPERTIES_FB };
	sys_props.next = &props2;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemBodyTrackingPropertiesFB";
	table.spec         = "XrSystemBodyTrackingPropertiesFB";
	table.cols[0].add({"supportsBodyTracking"});
	table.cols[1].add({props2.supportsBodyTracking ? "True":"False"});
	tables->add(table);

	// XR_HTC_body_tracking
	XrSystemBodyTrackingPropertiesHTC props3 = { XR_TYPE_SYSTEM_BODY_TRACKING_PROPERTIES_HTC };
	sys_props.next = &props3;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemBodyTrackingPropertiesHTC";
	table.spec         = "XrSystemBodyTrackingPropertiesHTC";
	table.cols[0].add({"supportsBodyTracking"});
	table.cols[1].add({props3.supportsBodyTracking ? "True":"False"});
	tables->add(table);

	// XR_META_colocation_discovery
	XrSystemColocationDiscoveryPropertiesMETA props4 = { XR_TYPE_SYSTEM_COLOCATION_DISCOVERY_PROPERTIES_META };
	sys_props.next = &props4;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemColocationDiscoveryPropertiesMETA";
	table.spec         = "XrSystemColocationDiscoveryPropertiesMETA";
	table.cols[0].add({"supportsColocationDiscovery"});
	table.cols[1].add({props4.supportsColocationDiscovery ? "True":"False"});
	tables->add(table);

	// XR_FB_color_space
	XrSystemColorSpacePropertiesFB props5 = { XR_TYPE_SYSTEM_COLOR_SPACE_PROPERTIES_FB };
	sys_props.next = &props5;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemColorSpacePropertiesFB";
	table.spec         = "XrSystemColorSpacePropertiesFB";
	table.cols[0].add({"colorSpace"});
	table.cols[1].add({"N/I"});
	tables->add(table);

	// XR_META_environment_depth
	XrSystemEnvironmentDepthPropertiesMETA props6 = { XR_TYPE_SYSTEM_ENVIRONMENT_DEPTH_PROPERTIES_META };
	sys_props.next = &props6;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemEnvironmentDepthPropertiesMETA";
	table.spec         = "XrSystemEnvironmentDepthPropertiesMETA";
	table.cols[0].add({"supportsEnvironmentDepth"});
	table.cols[1].add({props6.supportsEnvironmentDepth ? "True":"False"});
	table.cols[0].add({"supportsHandRemoval"});
	table.cols[1].add({props6.supportsHandRemoval ? "True":"False"});
	tables->add(table);

	// XR_EXT_eye_gaze_interaction
	XrSystemEyeGazeInteractionPropertiesEXT props7 = { XR_TYPE_SYSTEM_EYE_GAZE_INTERACTION_PROPERTIES_EXT };
	sys_props.next = &props7;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemEyeGazeInteractionPropertiesEXT";
	table.spec         = "XrSystemEyeGazeInteractionPropertiesEXT";
	table.cols[0].add({"supportsEyeGazeInteraction"});
	table.cols[1].add({props7.supportsEyeGazeInteraction ? "True":"False"});
	tables->add(table);

	// XR_FB_eye_tracking_social
	XrSystemEyeTrackingPropertiesFB props8 = { XR_TYPE_SYSTEM_EYE_TRACKING_PROPERTIES_FB };
	sys_props.next = &props8;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemEyeTrackingPropertiesFB";
	table.spec         = "XrSystemEyeTrackingPropertiesFB";
	table.cols[0].add({"supportsEyeTracking"});
	table.cols[1].add({props8.supportsEyeTracking ? "True":"False"});
	tables->add(table);

	// XR_FB_face_tracking2
	XrSystemFaceTrackingProperties2FB props9 = { XR_TYPE_SYSTEM_FACE_TRACKING_PROPERTIES2_FB };
	sys_props.next = &props9;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFaceTrackingProperties2FB";
	table.spec         = "XrSystemFaceTrackingProperties2FB";
	table.cols[0].add({"supportsVisualFaceTracking"});
	table.cols[1].add({props9.supportsVisualFaceTracking ? "True":"False"});
	table.cols[0].add({"supportsAudioFaceTracking"});
	table.cols[1].add({props9.supportsAudioFaceTracking ? "True":"False"});
	tables->add(table);

	// XR_FB_face_tracking
	XrSystemFaceTrackingPropertiesFB props10 = { XR_TYPE_SYSTEM_FACE_TRACKING_PROPERTIES_FB };
	sys_props.next = &props10;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFaceTrackingPropertiesFB";
	table.spec         = "XrSystemFaceTrackingPropertiesFB";
	table.cols[0].add({"supportsFaceTracking"});
	table.cols[1].add({props10.supportsFaceTracking ? "True":"False"});
	tables->add(table);

	// XR_ML_facial_expression
	XrSystemFacialExpressionPropertiesML props11 = { XR_TYPE_SYSTEM_FACIAL_EXPRESSION_PROPERTIES_ML };
	sys_props.next = &props11;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFacialExpressionPropertiesML";
	table.spec         = "XrSystemFacialExpressionPropertiesML";
	table.cols[0].add({"supportsFacialExpression"});
	table.cols[1].add({props11.supportsFacialExpression ? "True":"False"});
	tables->add(table);

	// XR_HTC_facial_tracking
	XrSystemFacialTrackingPropertiesHTC props12 = { XR_TYPE_SYSTEM_FACIAL_TRACKING_PROPERTIES_HTC };
	sys_props.next = &props12;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFacialTrackingPropertiesHTC";
	table.spec         = "XrSystemFacialTrackingPropertiesHTC";
	table.cols[0].add({"supportEyeFacialTracking"});
	table.cols[1].add({props12.supportEyeFacialTracking ? "True":"False"});
	table.cols[0].add({"supportLipFacialTracking"});
	table.cols[1].add({props12.supportLipFacialTracking ? "True":"False"});
	tables->add(table);

	// XR_MNDX_force_feedback_curl
	XrSystemForceFeedbackCurlPropertiesMNDX props13 = { XR_TYPE_SYSTEM_FORCE_FEEDBACK_CURL_PROPERTIES_MNDX };
	sys_props.next = &props13;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemForceFeedbackCurlPropertiesMNDX";
	table.spec         = "XrSystemForceFeedbackCurlPropertiesMNDX";
	table.cols[0].add({"supportsForceFeedbackCurl"});
	table.cols[1].add({props13.supportsForceFeedbackCurl ? "True":"False"});
	tables->add(table);

	// XR_VARJO_foveated_rendering
	XrSystemFoveatedRenderingPropertiesVARJO props14 = { XR_TYPE_SYSTEM_FOVEATED_RENDERING_PROPERTIES_VARJO };
	sys_props.next = &props14;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFoveatedRenderingPropertiesVARJO";
	table.spec         = "XrSystemFoveatedRenderingPropertiesVARJO";
	table.cols[0].add({"supportsFoveatedRendering"});
	table.cols[1].add({props14.supportsFoveatedRendering ? "True":"False"});
	tables->add(table);

	// XR_META_foveation_eye_tracked
	XrSystemFoveationEyeTrackedPropertiesMETA props15 = { XR_TYPE_SYSTEM_FOVEATION_EYE_TRACKED_PROPERTIES_META };
	sys_props.next = &props15;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemFoveationEyeTrackedPropertiesMETA";
	table.spec         = "XrSystemFoveationEyeTrackedPropertiesMETA";
	table.cols[0].add({"supportsFoveationEyeTracked"});
	table.cols[1].add({props15.supportsFoveationEyeTracked ? "True":"False"});
	tables->add(table);

	// XR_MSFT_hand_tracking_mesh
	XrSystemHandTrackingMeshPropertiesMSFT props16 = { XR_TYPE_SYSTEM_HAND_TRACKING_MESH_PROPERTIES_MSFT };
	sys_props.next = &props16;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemHandTrackingMeshPropertiesMSFT";
	table.spec         = "XrSystemHandTrackingMeshPropertiesMSFT";
	table.cols[0].add({"supportsHandTrackingMesh"});
	table.cols[1].add({props16.supportsHandTrackingMesh ? "True":"False"});
	table.cols[0].add({"maxHandMeshIndexCount"});
	table.cols[1].add({new_string("%u", props16.maxHandMeshIndexCount)});
	table.cols[0].add({"maxHandMeshVertexCount"});
	table.cols[1].add({new_string("%u", props16.maxHandMeshVertexCount)});
	tables->add(table);

	// XR_EXT_hand_tracking
	XrSystemHandTrackingPropertiesEXT props17 = { XR_TYPE_SYSTEM_HAND_TRACKING_PROPERTIES_EXT };
	sys_props.next = &props17;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemHandTrackingPropertiesEXT";
	table.spec         = "XrSystemHandTrackingPropertiesEXT";
	table.cols[0].add({"supportsHandTracking"});
	table.cols[1].add({props17.supportsHandTracking ? "True":"False"});
	tables->add(table);

	// XR_META_headset_id
	XrSystemHeadsetIdPropertiesMETA props18 = { XR_TYPE_SYSTEM_HEADSET_ID_PROPERTIES_META };
	sys_props.next = &props18;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemHeadsetIdPropertiesMETA";
	table.spec         = "XrSystemHeadsetIdPropertiesMETA";
	table.cols[0].add({"id"});
	table.cols[1].add({"N/I"});
	tables->add(table);

	// XR_FB_keyboard_tracking
	XrSystemKeyboardTrackingPropertiesFB props19 = { XR_TYPE_SYSTEM_KEYBOARD_TRACKING_PROPERTIES_FB };
	sys_props.next = &props19;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemKeyboardTrackingPropertiesFB";
	table.spec         = "XrSystemKeyboardTrackingPropertiesFB";
	table.cols[0].add({"supportsKeyboardTracking"});
	table.cols[1].add({props19.supportsKeyboardTracking ? "True":"False"});
	tables->add(table);

	// XR_VARJO_marker_tracking
	XrSystemMarkerTrackingPropertiesVARJO props20 = { XR_TYPE_SYSTEM_MARKER_TRACKING_PROPERTIES_VARJO };
	sys_props.next = &props20;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemMarkerTrackingPropertiesVARJO";
	table.spec         = "XrSystemMarkerTrackingPropertiesVARJO";
	table.cols[0].add({"supportsMarkerTracking"});
	table.cols[1].add({props20.supportsMarkerTracking ? "True":"False"});
	tables->add(table);

	// XR_ML_marker_understanding
	XrSystemMarkerUnderstandingPropertiesML props21 = { XR_TYPE_SYSTEM_MARKER_UNDERSTANDING_PROPERTIES_ML };
	sys_props.next = &props21;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemMarkerUnderstandingPropertiesML";
	table.spec         = "XrSystemMarkerUnderstandingPropertiesML";
	table.cols[0].add({"supportsMarkerUnderstanding"});
	table.cols[1].add({props21.supportsMarkerUnderstanding ? "True":"False"});
	tables->add(table);

	// XR_META_passthrough_color_lut
	XrSystemPassthroughColorLutPropertiesMETA props22 = { XR_TYPE_SYSTEM_PASSTHROUGH_COLOR_LUT_PROPERTIES_META };
	sys_props.next = &props22;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemPassthroughColorLutPropertiesMETA";
	table.spec         = "XrSystemPassthroughColorLutPropertiesMETA";
	table.cols[0].add({"maxColorLutResolution"});
	table.cols[1].add({new_string("%u", props22.maxColorLutResolution)});
	tables->add(table);

	// XR_FB_passthrough
	XrSystemPassthroughProperties2FB props23 = { XR_TYPE_SYSTEM_PASSTHROUGH_PROPERTIES2_FB };
	sys_props.next = &props23;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemPassthroughProperties2FB";
	table.spec         = "XrSystemPassthroughProperties2FB";
	table.cols[0].add({"capabilities"});
	table.cols[1].add({"N/I"});
	tables->add(table);

	// XR_FB_passthrough
	XrSystemPassthroughPropertiesFB props24 = { XR_TYPE_SYSTEM_PASSTHROUGH_PROPERTIES_FB };
	sys_props.next = &props24;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemPassthroughPropertiesFB";
	table.spec         = "XrSystemPassthroughPropertiesFB";
	table.cols[0].add({"supportsPassthrough"});
	table.cols[1].add({props24.supportsPassthrough ? "True":"False"});
	tables->add(table);

	// XR_EXT_plane_detection
	XrSystemPlaneDetectionPropertiesEXT props25 = { XR_TYPE_SYSTEM_PLANE_DETECTION_PROPERTIES_EXT };
	sys_props.next = &props25;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemPlaneDetectionPropertiesEXT";
	table.spec         = "XrSystemPlaneDetectionPropertiesEXT";
	table.cols[0].add({"supportedFeatures"});
	table.cols[1].add({"N/I"});
	tables->add(table);

	// XR_META_body_tracking_full_body
	XrSystemPropertiesBodyTrackingFullBodyMETA props26 = { XR_TYPE_SYSTEM_PROPERTIES_BODY_TRACKING_FULL_BODY_META };
	sys_props.next = &props26;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemPropertiesBodyTrackingFullBodyMETA";
	table.spec         = "XrSystemPropertiesBodyTrackingFullBodyMETA";
	table.cols[0].add({"supportsFullBodyTracking"});
	table.cols[1].add({props26.supportsFullBodyTracking ? "True":"False"});
	tables->add(table);

	// XR_FB_render_model
	XrSystemRenderModelPropertiesFB props27 = { XR_TYPE_SYSTEM_RENDER_MODEL_PROPERTIES_FB };
	sys_props.next = &props27;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemRenderModelPropertiesFB";
	table.spec         = "XrSystemRenderModelPropertiesFB";
	table.cols[0].add({"supportsRenderModelLoading"});
	table.cols[1].add({props27.supportsRenderModelLoading ? "True":"False"});
	tables->add(table);

	// XR_META_simultaneous_hands_and_controllers
	XrSystemSimultaneousHandsAndControllersPropertiesMETA props28 = { XR_TYPE_SYSTEM_SIMULTANEOUS_HANDS_AND_CONTROLLERS_PROPERTIES_META };
	sys_props.next = &props28;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSimultaneousHandsAndControllersPropertiesMETA";
	table.spec         = "XrSystemSimultaneousHandsAndControllersPropertiesMETA";
	table.cols[0].add({"supportsSimultaneousHandsAndControllers"});
	table.cols[1].add({props28.supportsSimultaneousHandsAndControllers ? "True":"False"});
	tables->add(table);

	// XR_FB_space_warp
	XrSystemSpaceWarpPropertiesFB props29 = { XR_TYPE_SYSTEM_SPACE_WARP_PROPERTIES_FB };
	sys_props.next = &props29;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpaceWarpPropertiesFB";
	table.spec         = "XrSystemSpaceWarpPropertiesFB";
	table.cols[0].add({"recommendedMotionVectorImageRectWidth"});
	table.cols[1].add({new_string("%u", props29.recommendedMotionVectorImageRectWidth)});
	table.cols[0].add({"recommendedMotionVectorImageRectHeight"});
	table.cols[1].add({new_string("%u", props29.recommendedMotionVectorImageRectHeight)});
	tables->add(table);

	// XR_BD_spatial_anchor
	XrSystemSpatialAnchorPropertiesBD props30 = { XR_TYPE_SYSTEM_SPATIAL_ANCHOR_PROPERTIES_BD };
	sys_props.next = &props30;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialAnchorPropertiesBD";
	table.spec         = "XrSystemSpatialAnchorPropertiesBD";
	table.cols[0].add({"supportsSpatialAnchor"});
	table.cols[1].add({props30.supportsSpatialAnchor ? "True":"False"});
	tables->add(table);

	// XR_BD_spatial_anchor_sharing
	XrSystemSpatialAnchorSharingPropertiesBD props31 = { XR_TYPE_SYSTEM_SPATIAL_ANCHOR_SHARING_PROPERTIES_BD };
	sys_props.next = &props31;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialAnchorSharingPropertiesBD";
	table.spec         = "XrSystemSpatialAnchorSharingPropertiesBD";
	table.cols[0].add({"supportsSpatialAnchorSharing"});
	table.cols[1].add({props31.supportsSpatialAnchorSharing ? "True":"False"});
	tables->add(table);

	// XR_META_spatial_entity_group_sharing
	XrSystemSpatialEntityGroupSharingPropertiesMETA props32 = { XR_TYPE_SYSTEM_SPATIAL_ENTITY_GROUP_SHARING_PROPERTIES_META };
	sys_props.next = &props32;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialEntityGroupSharingPropertiesMETA";
	table.spec         = "XrSystemSpatialEntityGroupSharingPropertiesMETA";
	table.cols[0].add({"supportsSpatialEntityGroupSharing"});
	table.cols[1].add({props32.supportsSpatialEntityGroupSharing ? "True":"False"});
	tables->add(table);

	// XR_FB_spatial_entity
	XrSystemSpatialEntityPropertiesFB props33 = { XR_TYPE_SYSTEM_SPATIAL_ENTITY_PROPERTIES_FB };
	sys_props.next = &props33;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialEntityPropertiesFB";
	table.spec         = "XrSystemSpatialEntityPropertiesFB";
	table.cols[0].add({"supportsSpatialEntity"});
	table.cols[1].add({props33.supportsSpatialEntity ? "True":"False"});
	tables->add(table);

	// XR_META_spatial_entity_sharing
	XrSystemSpatialEntitySharingPropertiesMETA props34 = { XR_TYPE_SYSTEM_SPATIAL_ENTITY_SHARING_PROPERTIES_META };
	sys_props.next = &props34;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialEntitySharingPropertiesMETA";
	table.spec         = "XrSystemSpatialEntitySharingPropertiesMETA";
	table.cols[0].add({"supportsSpatialEntitySharing"});
	table.cols[1].add({props34.supportsSpatialEntitySharing ? "True":"False"});
	tables->add(table);

	// XR_BD_spatial_mesh
	XrSystemSpatialMeshPropertiesBD props35 = { XR_TYPE_SYSTEM_SPATIAL_MESH_PROPERTIES_BD };
	sys_props.next = &props35;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialMeshPropertiesBD";
	table.spec         = "XrSystemSpatialMeshPropertiesBD";
	table.cols[0].add({"supportsSpatialMesh"});
	table.cols[1].add({props35.supportsSpatialMesh ? "True":"False"});
	tables->add(table);

	// XR_BD_spatial_plane
	XrSystemSpatialPlanePropertiesBD props36 = { XR_TYPE_SYSTEM_SPATIAL_PLANE_PROPERTIES_BD };
	sys_props.next = &props36;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialPlanePropertiesBD";
	table.spec         = "XrSystemSpatialPlanePropertiesBD";
	table.cols[0].add({"supportsSpatialPlane"});
	table.cols[1].add({props36.supportsSpatialPlane ? "True":"False"});
	tables->add(table);

	// XR_BD_spatial_scene
	XrSystemSpatialScenePropertiesBD props37 = { XR_TYPE_SYSTEM_SPATIAL_SCENE_PROPERTIES_BD };
	sys_props.next = &props37;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialScenePropertiesBD";
	table.spec         = "XrSystemSpatialScenePropertiesBD";
	table.cols[0].add({"supportsSpatialScene"});
	table.cols[1].add({props37.supportsSpatialScene ? "True":"False"});
	tables->add(table);

	// XR_BD_spatial_sensing
	XrSystemSpatialSensingPropertiesBD props38 = { XR_TYPE_SYSTEM_SPATIAL_SENSING_PROPERTIES_BD };
	sys_props.next = &props38;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemSpatialSensingPropertiesBD";
	table.spec         = "XrSystemSpatialSensingPropertiesBD";
	table.cols[0].add({"supportsSpatialSensing"});
	table.cols[1].add({props38.supportsSpatialSensing ? "True":"False"});
	tables->add(table);

	// XR_EXT_user_presence
	XrSystemUserPresencePropertiesEXT props39 = { XR_TYPE_SYSTEM_USER_PRESENCE_PROPERTIES_EXT };
	sys_props.next = &props39;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemUserPresencePropertiesEXT";
	table.spec         = "XrSystemUserPresencePropertiesEXT";
	table.cols[0].add({"supportsUserPresence"});
	table.cols[1].add({props39.supportsUserPresence ? "True":"False"});
	tables->add(table);

	// XR_META_virtual_keyboard
	XrSystemVirtualKeyboardPropertiesMETA props40 = { XR_TYPE_SYSTEM_VIRTUAL_KEYBOARD_PROPERTIES_META };
	sys_props.next = &props40;
	error = xrGetSystemProperties(instance, system_id, &sys_props);
	if (XR_FAILED(error)) properties_err = openxr_result_string(error);
	table = {};
	table.error        = properties_err;
	table.tag          = display_tag_properties;
	table.show_type    = true;
	table.column_count = 2;
	table.name_func    = "xrGetSystemProperties";
	table.name_type    = "XrSystemVirtualKeyboardPropertiesMETA";
	table.spec         = "XrSystemVirtualKeyboardPropertiesMETA";
	table.cols[0].add({"supportsVirtualKeyboard"});
	table.cols[1].add({props40.supportsVirtualKeyboard ? "True":"False"});
	tables->add(table);

// <<GENERATED_CODE_END>>
}


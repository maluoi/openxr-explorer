project(openxr-explorer VERSION 1.0
                        DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI."
                        LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#### OpenXR - https://www.khronos.org/openxr/ ####
CPMAddPackage(
  NAME OpenXR
  VERSION 1.1.47
  GITHUB_REPOSITORY KhronosGroup/OpenXR-SDK
  GIT_TAG 858912260ca616f4c23f7fb61c89228c353eb124 # v1.1.47
)

if (UNIX)
    find_package(X11 REQUIRED)
    find_package(GLEW REQUIRED)
    set(PLATFORM_FILES
        imgui/imgui_impl_x11.h
        imgui/imgui_impl_x11.cpp
    )
endif(UNIX)

if (WIN32)
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
    set(PLATFORM_FILES
        resource.h
        resource.rc
        imgui/imgui_impl_win32.h
        imgui/imgui_impl_win32.cpp
    )
endif(WIN32)

add_executable(openxr-explorer 
    main.cpp
    openxr_info.h
    openxr_info.cpp
    openxr_properties.h
    openxr_properties.cpp
    app_cli.h
    app_cli.cpp
    app_imgui.h
    app_imgui.cpp
    array.h
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_skg.h
    imgui/imgui_impl_skg.cpp
    imgui/imgui_internal.h
    imgui/imgui_shader.hlsl.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/sk_gpu.h
    imgui/sokol_time.h
    ${PLATFORM_FILES})

target_include_directories(openxr-explorer PRIVATE
    ../common
    ${OpenXR_INCLUDE_DIRS})

if (UNIX)
    set(LINUX_LIBS 
        X11
        X11-xcb
        ${X11_LIBRARIES}
        xcb
        xcb-xkb
        xcb-randr
        xcb-xfixes
        xcb-cursor
        xcb-keysyms
        GL
        GLEW
        GLX )
endif(UNIX)

target_link_libraries(openxr-explorer 
    PUBLIC
    xrruntime 
    PRIVATE
    openxr_loader
    ${LINUX_LIBS})
project(openxr-explorer VERSION 1.0
                        DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI."
                        LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

find_package(OpenXR REQUIRED)
find_package(X11 REQUIRED)
find_package(GLEW REQUIRED)

add_executable(openxr-explorer 
    main.cpp
    openxr_info.h
    openxr_info.cpp
    app_cli.h
    app_cli.cpp
    array.h
    imgui/imconfig.h
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_skg.h
    imgui/imgui_impl_skg.cpp
    imgui/imgui_impl_win32.h
    imgui/imgui_impl_win32.cpp
    imgui/imgui_impl_x11.h
    imgui/imgui_impl_x11.cpp
    imgui/imgui_internal.h
    imgui/imgui_shader.hlsl.h
    imgui/imgui_skg.h
    imgui/imgui_skg.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/sk_gpu.h
    imgui/sokol_time.h)

target_include_directories(openxr-explorer PRIVATE
    ../common
    ${OPENXR_OPENXR_INCLUDE_DIR})

target_link_libraries(openxr-explorer 
    xrruntime 
    openxr_loader
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
    GLX)
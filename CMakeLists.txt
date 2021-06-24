cmake_minimum_required(VERSION 3.7)

project(openxrexplorer VERSION 1.0
                       DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI. "
                       LANGUAGES CXX)

set(OPENXR_SDK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external/OpenXR-SDK")
set(OPENXR_SDK_BUILD_DIR "${CMAKE_BINARY_DIR}/src/external/OpenXR-SDK")
set(OPENXR_loader_LIBRARY "${CMAKE_BINARY_DIR}/src/loader/Release/openxr_loader.lib")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)

install(TARGETS openxr-explorer xrsetruntime
        DESTINATION bin)

install(FILES src/common/xr_runtimes.txt DESTINATION bin)

# sudo cmake --install . --prefix /usr
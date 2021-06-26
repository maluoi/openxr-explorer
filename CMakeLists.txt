cmake_minimum_required(VERSION 3.7)

project(openxrexplorer VERSION 1.0
                       DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI. "
                       LANGUAGES CXX)

# Default build configuration to RelWithDebInfo 
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
    endif()
endif()

set(OPENXR_SDK_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/external/OpenXR-SDK")
set(OPENXR_SDK_BUILD_DIR "${CMAKE_BINARY_DIR}/src/external/OpenXR-SDK")
set(OPENXR_loader_LIBRARY "${CMAKE_BINARY_DIR}/src/loader/Release/openxr_loader.lib")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(src)

install(TARGETS openxr-explorer xrsetruntime
        DESTINATION bin)

# This is not really recommended yet, as it also installs
# the OpenXR loader. Doing that could override an existing
# installed OpenXR loader. Shared libraries are better on Linux.
#
# Command to install via cmake:
#     sudo cmake --install . --prefix /usr/local
#
# Command to uninstall afterwards:
#     sudo xargs rm < install_manifest.txt
cmake_minimum_required(VERSION 3.7)

project(openxrexplorer VERSION 1.7
                       DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI. "
                       LANGUAGES CXX)

# Default build configuration to RelWithDebInfo 
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
    endif()
endif()

# CMake 3.14 introduced CMP0083 - Control generation of Position Independent Executable (PIE)
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14.0")
	cmake_policy(SET CMP0083 NEW)
	include(CheckPIESupported)
	check_pie_supported()
endif()

# Default to PIC code
include(CheckPIESupported)
check_pie_supported()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# See: https://github.com/cpm-cmake/CPM.cmake
include("cmake/CPM.cmake")
if (UNIX)
  # Linux users/devs prefer shared system libraries rather than
  # static libraries. Linux OpenXR runtimes are responsible for
  # providing the shared OpenXR loader there. On Windows and
  # Android, static linking is necessary.
  set(CPM_USE_LOCAL_PACKAGES ON) # Tell CPM to prefer locally installed packages
endif()
include(cmake/json.cmake)

add_subdirectory(src)

install(TARGETS openxr-explorer xrsetruntime
        DESTINATION bin)

# CLI commands for installation on Linux
#
# Command to install via cmake:
#     sudo cmake --install . --prefix /usr/local
#
# Command to uninstall afterwards:
#     sudo xargs rm < install_manifest.txt

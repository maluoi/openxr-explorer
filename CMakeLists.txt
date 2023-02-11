cmake_minimum_required(VERSION 3.7)

project(openxrexplorer VERSION 1.5
                       DESCRIPTION "A cross-platform OpenXR capabilities explorer and runtime switcher with a CLI and GUI."
                       LANGUAGES CXX)

# Default build configuration to RelWithDebInfo 
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "" FORCE)
    endif()
endif()

# See: https://github.com/cpm-cmake/CPM.cmake
include("cmake/CPM.cmake")
if (UNIX)
  # Linux users/devs prefer shared system libraries rather than
  # static libraries. Linux OpenXR runtimes are responsible for
  # providing the shared OpenXR loader there. On Windows and
  # Android, static linking is necessary.
  set(CPM_USE_LOCAL_PACKAGES ON) # Tell CPM to prefer locally installed packages
endif()

# For creating application installer
set(CPACK_PACKAGE_NAME                 "openxr-explorer")
set(CPACK_PACKAGE_VENDOR               "openxr-explorer")
set(CPACK_PACKAGE_INSTALL_DIRECTORY    "openxr-explorer")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "openxr-explorer")
set(CPACK_PACKAGE_VERSION              ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION          ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY  ${PROJECT_DESCRIPTION})
set(CPACK_PACKAGE_HOMEPAGE_URL         "https://github.com/maluoi/openxr-explorer")
set(CPACK_PACKAGE_EXECUTABLES          openxr-explorer;openxr-explorer)
set(CPACK_VERBATIM_VARIABLES           "TRUE")
set(CPACK_NSIS_INSTALLED_ICON_NAME     "bin/openxr-explorer.exe")
set(CPACK_NSIS_MODIFY_PATH             "ON")
set(CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS [[StrCpy $ADD_TO_PATH_CURRENT_USER "1"]])
set(CPACK_NSIS_IGNORE_LICENSE_PAGE     "ON")
set(CPACK_WIX_UPGRADE_GUID             "B6CC719C-1F9D-486B-B715-40B9BB61BA11")
set(CPACK_WIX_PRODUCT_ICON             "${CMAKE_CURRENT_SOURCE_DIR}/src/openxrexplorer/oxr-explorer-icon.ico")
set(CPACK_WIX_PATCH_FILE               "${CMAKE_CURRENT_SOURCE_DIR}/cmake/AddPath.xml")
set(CPACK_RESOURCE_FILE_LICENSE        "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.TXT")
set(CPACK_GENERATOR "WIX")

include(CPack)
add_subdirectory(src)

install(TARGETS openxr-explorer xrsetruntime)

# CLI commands for installation on Linux
#
# Command to install via cmake:
#     sudo cmake --install . --prefix /usr/local
#
# Command to uninstall afterwards:
#     sudo xargs rm < install_manifest.txt

# You can also use this command to create an installer:
#     Using WiX v3: https://wixtoolset.org/docs/wix3/
#     cpack -G WIX
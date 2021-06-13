project(xrsetruntime VERSION 1.0
                     DESCRIPTION "Sets the OpenXR runtime"
                     LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../common)
add_executable(xrsetruntime 
    xrruntime_main.cpp)
target_link_libraries(xrsetruntime xrruntime)

add_custom_command(
    TARGET xrsetruntime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/../common/xr_runtimes.txt
            $<TARGET_FILE_DIR:xrsetruntime>/xr_runtimes.txt
    COMMENT "Copy xr_runtimes.txt to exe folder" VERBATIM)
project(xrsetruntime VERSION 1.7
                     DESCRIPTION "Sets the OpenXR runtime"
                     LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

include_directories(../common)
add_executable(xrsetruntime 
    xrruntime_main.cpp)
target_link_libraries(xrsetruntime xrruntime)
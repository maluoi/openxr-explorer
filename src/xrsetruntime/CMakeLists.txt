project(xrsetruntime VERSION 1.0
                     DESCRIPTION "Sets the OpenXR runtime"
                     LANGUAGES CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(xrsetruntime 
    xrruntime_main.cpp)
target_link_libraries(xrsetruntime xrruntime)
target_include_directories(xrsetruntime PRIVATE ../common)
project(xrruntime VERSION 1.0
                  DESCRIPTION "XR runtime list loading code shared between both exe's"
                  LANGUAGES CXX)

include_directories(${CMAKE_SOURCE_DIR})
add_library(xrruntime STATIC xrruntime.cpp xrruntime.h)
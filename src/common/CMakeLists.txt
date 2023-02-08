project(xrruntime VERSION 1.0
                  DESCRIPTION "XR runtime list loading code shared between both exe's"
                  LANGUAGES CXX)

add_library(xrruntime STATIC xrruntime.cpp xrruntime.h)
target_include_directories(xrruntime PRIVATE ${CMAKE_SOURCE_DIR})
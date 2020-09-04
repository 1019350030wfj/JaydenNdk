cmake_minimum_required(VERSION 3.4.1)
#set(CMAKE_VERBOSE_MAKEFILE on)
#set(lib_src_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#
#set(lib_build_DIR $ENV{HOME}/tmp/${ANDROID_ABI})
#file(MAKE_DIRECTORY ${lib_build_DIR})
#
#add_subdirectory(${lib_src_DIR}/gmath ${lib_build_DIR}/gmath)
#add_subdirectory(${lib_src_DIR}/gperf ${lib_build_DIR}/gperf)

set(distributionDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../distribution)

add_library(lib_gmath STATIC IMPORTED)
set_target_properties(lib_gmath PROPERTIES IMPORTED_LOCATION
        ${distributionDir}/gmath/lib/${ANDROID_ABI}/libgmath.a)

add_library(lib_gperf SHARED IMPORTED)
set_target_properties(lib_gperf PROPERTIES IMPORTED_LOCATION
        ${distributionDir}/gperf/lib/${ANDROID_ABI}/libgperf.so)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

add_library(hello-libs SHARED hello-libs.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/source/LearnPThread.cpp)

#include_directories 用来导入相关头文件
target_include_directories(hello-libs PRIVATE
                           ${distributionDir}/gmath/include
                           ${distributionDir}/gperf/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 将导入的库和自己的原生库关联起来
target_link_libraries(hello-libs
                      android
                      lib_gmath
                      lib_gperf
                      log)
# 基本框架
# - minimun version

# 中间可能还需要引入其他静态库和动态库

# - add_library(库名称 动态or静态 源文件）
# - target_link_libraries（库名称 其他链接库）
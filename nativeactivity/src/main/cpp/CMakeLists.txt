cmake_minimum_required(VERSION 3.4.1)

#编译naive_app_glue为一个静态库
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

#开始构建app的动态库
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall -Werror")

set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
add_library(native-activity SHARED main.cpp)

#配置头文件
target_include_directories(native-activity PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue)

#添加依赖库
target_link_libraries(native-activity
        android
        native_app_glue
        EGL
        GLESv1_CM
        log)
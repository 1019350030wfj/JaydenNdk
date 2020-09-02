cmake_minimum_required(VERSION 3.4.1)

#设置c++版本
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")

set(distributionDir ${CMAKE_CURRENT_SOURCE_DIR}/../../../distribution)
set(libName learn-ffmpeg)
#====================== 其他静态库 和 动态库 =========================

# 导入相关头文件
include_directories(${distributionDir}/ffmpeg/include
        ${CMAKE_CURRENT_SOURCE_DIR}/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/include/util
        ${CMAKE_CURRENT_SOURCE_DIR}/include/player
        ${CMAKE_CURRENT_SOURCE_DIR}/include/player/decoder
        ${CMAKE_CURRENT_SOURCE_DIR}/include/player/render
        ${CMAKE_CURRENT_SOURCE_DIR}/include/player/render/audio
        ${CMAKE_CURRENT_SOURCE_DIR}/include/player/render/video)

#静态库
link_directories(${distributionDir}/ffmpeg/lib/${ANDROID_ABI})

# 源文件 .cpp .c
file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/source/player/decoder/*.cpp
        ${CMAKE_SOURCE_DIR}/source/render/*.cpp
        ${CMAKE_SOURCE_DIR}/source/util/*.cpp
        ${CMAKE_SOURCE_DIR}/source/render/audio/*.cpp
        ${CMAKE_SOURCE_DIR}/source/render/video/*.cpp
        ${CMAKE_SOURCE_DIR}/source/*.cpp
        ${CMAKE_SOURCE_DIR}/*.cpp)

#====================== 其他静态库 和 动态库 =========================

add_library(${libName} SHARED ${src-files})

set(third-party-libs
        avformat
        avcodec
        avfilter
        swresample
        swscale
        avutil
        )

set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )

target_link_libraries(${libName}
        ${log-lib}
        ${third-party-libs}
        ${native-libs}
        )

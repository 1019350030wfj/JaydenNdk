cmake_minimum_required(VERSION 3.4.1)

include_directories(util
                    glm
                    render
                    sample)

file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/sample/*.cpp
        ${CMAKE_SOURCE_DIR}/util/*.cpp
        ${CMAKE_SOURCE_DIR}/render/*.cpp)

add_library(native-render
            SHARED
            ${src-files})

find_library(# Sets the name of the path variable.
             log-lib

             # Specifies the name of the NDK library that
             # you want CMake to locate.
             log)

target_link_libraries(native-render
                      ${log-lib}
                      EGL
                      GLESv3)
cmake_minimum_required(VERSION 3.4.1)

#头文件
include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/third/include)

set(jnilibs "${CMAKE_SOURCE_DIR}/src/main/jniLib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${jnilibs}/${ANDROID_ABI})

#使用c++14标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++14 -pthread -DMGULK_LOG_STDERR=1 -Wall -Wextra -Wnon-virtual-dtor -g")

add_subdirectory(${CMAKE_SOURCE_DIR}/src/main/cpp)
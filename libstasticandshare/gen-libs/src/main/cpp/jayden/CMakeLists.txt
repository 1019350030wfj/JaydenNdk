cmake_minimum_required(VERSION 3.4.1)

add_library(jayden STATIC jayden.cpp)

FIND_LIBRARY(log-lib log)

target_link_libraries(jayden ${log-lib})

set_target_properties(jayden
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY
                      "${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}")
cmake_minimum_required(VERSION 3.4.1)
add_library(yang SHARED yangyang.cpp)

find_library(log-lib log)

target_link_libraries(yang ${log-lib})
set_target_properties(yang
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}")
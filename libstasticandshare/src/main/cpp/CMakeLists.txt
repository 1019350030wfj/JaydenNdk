add_library(native SHARED native.cpp)

#引入.so文件
add_library(yang SHARED IMPORTED)
SET_TARGET_PROPERTIES(yang PROPERTIES IMPORTED_LOCATION "${jnilibs}/${ANDROID_ABI}/libyang.so")

#引入.a文件
add_library(jayden STATIC IMPORTED)
SET_TARGET_PROPERTIES(jayden PROPERTIES IMPORTED_LOCATION "${jnilibs}/${ANDROID_ABI}/libjayden.a")

find_library(log-lib log)

target_link_libraries(native yang jayden ${log-lib})
//
// Created by jayden on 2020/9/3.
//

#ifndef NATIVEDOING_YANGYANGY_H
#define NATIVEDOING_YANGYANGY_H

#include <stddef.h>
#include <android/log.h>
#include <jni.h>
#include <string>

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "yangyang", "__VA_ARGS__")

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    std::string (*getResponse)();//函数指针
    std::int32_t (*getYangYangKey)();
} YangYang_FUN;

extern __attribute__ ((visibility ("default"))) YangYang_FUN yangYangFun;

#ifdef __cplusplus
}
#endif

#endif //NATIVEDOING_YANGYANGY_H

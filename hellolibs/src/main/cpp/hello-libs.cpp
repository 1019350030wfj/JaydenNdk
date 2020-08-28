//
// Created by jayden on 2020/8/17.
//

#include <cstring>
#include <jni.h>
#include <cinttypes>
#include <gmath.h>
#include <gperf.h>
#include <string>
#include <android/log.h>

#define LOGI(...) \
    ((void)__android_log_print(ANDROID_LOG_INFO, "hello-libs::", __VA_ARGS__))

extern "C" JNIEXPORT jstring JNICALL
Java_com_meitu_hellolibs_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    auto ticks = GetTicks();
    for (auto exp = 0; exp < 32; exp++) {
        volatile unsigned val = gpower(exp);
        LOGI("exp %" PRIu64, val);
        (void)val;
    }
    ticks = GetTicks() - ticks;
    LOGI("calculation time: %" PRIu64, ticks);
    return env->NewStringUTF("Hello from JNI LIBS");
}

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
    //===========  native 访问 java 的成员变量
    //通过 JNIEnv 和对象 instance 实例拿到 class
    jclass javaClass = env->GetObjectClass(thiz);
    // 获取属性 field id
    jfieldID javaMember = env->GetFieldID(javaClass, "jayden", "I");
    //通过field id 获取属性值
    jint javaMemberValue = env->GetIntField(thiz, javaMember);
    LOGI("wfj native access java member jayden:%d", javaMemberValue);
    //===========  native 访问 java 的成员变量

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
extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_hellolibs_NativeAndJava_javaCallNative(JNIEnv *env, jobject thiz, jstring msg) {
    //=========== native 访问 java 静态变量 =================
    jclass javaClass = env->GetObjectClass(thiz);
    jfieldID staticField = env->GetStaticFieldID(javaClass, "sJayden", "I");
    jint staticValue = env->GetStaticIntField(javaClass, staticField);

    LOGI("wfj native access java static member staticValue:%d", staticValue);
    //=========== native 访问 java 静态变量 =================


}
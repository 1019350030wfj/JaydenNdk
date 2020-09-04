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

#include "LearnPThread.h"

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

    //=====  通过 class 来访问非静态方法 需要自己构造一个实例 ===========
    jclass class1 = env->FindClass("com/meitu/hellolibs/NativeAndJava");
    // 获取 java类的 构造函数id
    jmethodID constructed = env->GetMethodID(class1, "<init>", "()V");
    // 通过构造函数方法id 创建一个实例
    jobject nativeAndJava = env->NewObject(class1, constructed);
    // 获取 非静态方法 id
    jmethodID methodId = env->GetMethodID(class1, "nativeCallJava", "(Ljava/lang/String;)V");
    // new 一个 string  用于传递给java
    jstring nativeToJavaMsg = env->NewStringUTF("native to java");
    // 调用java 方法
    env->CallVoidMethod(nativeAndJava, methodId, nativeToJavaMsg);


    //===== 访问 java 静态方法 ===========
    jmethodID staticMethod = env->GetStaticMethodID(class1, "nativeCallJavaStaticMethod", "(Ljava/lang/String;)Ljava/lang/String;");
    jstring receiveJavaMsg = static_cast<jstring>(env->CallStaticObjectMethod(class1, staticMethod,
                                                                              env->NewStringUTF(
                                                                                      "native call java with msg")));
    // jstring to char*
    const char* cstr = env->GetStringUTFChars(receiveJavaMsg, NULL);
    LOGI("wfj %s", cstr);
    //释放资源
    env->ReleaseStringUTFChars(receiveJavaMsg, cstr);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_hellolibs_NativeThread_nativeCreateThread(JNIEnv *env, jobject thiz) {
    LearnPThread *pThread = new LearnPThread();
    pThread->createThreads();
    delete pThread;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_hellolibs_NativeThread_nativeMutexThread(JNIEnv *env, jobject thiz) {
    LearnPThread *pThread = new LearnPThread();
    pThread->mutexThreads();
    delete pThread;
}
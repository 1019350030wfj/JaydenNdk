//
// Created by jayden on 2020/9/23.
//

#include <jni.h>
#include "GlesPlay.h"

const JavaVM *javaVm = nullptr;
GlesPlay *glesPlay = nullptr;

int JNI_OnLoad(JavaVM *javaVm, void *pVoid) {
    ::javaVm = javaVm;
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_audiovideo_video_YUVPlayer_nativeWindowPlay(JNIEnv *env, jobject thiz,
        jstring yuv420p_path, jobject surface) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_audiovideo_video_YUVPlayer_nativeDestory(JNIEnv *env, jobject thiz) {
    if (glesPlay) {
        glesPlay->release();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_audiovideo_video_YUVPlayer_nativeGlesPlay(JNIEnv *env, jobject instance,
        jstring yuv420p_path, jobject surface) {
    const char * yuv420pPath = env->GetStringUTFChars(yuv420p_path, NULL);
    PlayCallback *callback = new PlayCallback(const_cast<JavaVM *>(javaVm), env, instance);
    glesPlay = new GlesPlay(env, instance, callback, yuv420pPath, surface);
    glesPlay->start();
    env->ReleaseStringUTFChars(yuv420p_path, yuv420pPath);
}
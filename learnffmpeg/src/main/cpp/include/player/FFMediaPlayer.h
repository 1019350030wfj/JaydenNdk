//
// Created by jayden on 2020/8/25.
//

#ifndef NATIVEDOING_FFMEDIAPLAYER_H
#define NATIVEDOING_FFMEDIAPLAYER_H


#include <jni.h>

class FFMediaPlayer {
public:
    FFMediaPlayer() {};
    ~FFMediaPlayer() {};

    void Init(JNIEnv *jniEnv, jobject obj, char* url, int renderType, jobject surface);
    void UnInit();

    void play();
    void pause();
    void stop();
    void seekToPosition(float position);
    long getMediaParams(int paramType);

private:
    JNIEnv *getJNIEnv(bool *isAttach);
    jobject getJavaObj();
    JavaVM *getJavaVM();

    static void postMessage(void *context, int msgType, float msgCode);

    JavaVM *m_JavaVM = nullptr;
    jobject m_JavaObj = nullptr;

//    VideoDe
};


#endif //NATIVEDOING_FFMEDIAPLAYER_H

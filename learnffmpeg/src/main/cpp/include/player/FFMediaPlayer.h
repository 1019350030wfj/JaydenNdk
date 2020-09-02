//
// Created by jayden on 2020/8/25.
//

#ifndef NATIVEDOING_FFMEDIAPLAYER_H
#define NATIVEDOING_FFMEDIAPLAYER_H


#include <jni.h>
#include "decoder/VideoDecoder.h"
#include "decoder/AudioDecoder.h"

#define VIDEO_RENDER_OPENGL 0
#define VIDEO_RENDER_ANWINDOW 1

#define JAVA_PLAYER_EVENT_CALLBACK_API_NAME "playerEventCallback"

#define MEDIA_PARAM_VIDEO_WIDTH         0x0001
#define MEDIA_PARAM_VIDEO_HEIGHT        0x0002
#define MEDIA_PARAM_VIDEO_DURATION      0x0003

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

    VideoDecoder *mVideoDecoder = nullptr;
    AudioDecoder *mAudioDecoder = nullptr;

    VideoRender *mVideoRender = nullptr;
    AudioRender *mAudioRender = nullptr;
};


#endif //NATIVEDOING_FFMEDIAPLAYER_H

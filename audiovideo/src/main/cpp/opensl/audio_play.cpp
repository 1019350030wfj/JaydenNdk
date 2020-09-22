//
// Created by jayden on 2020/9/22.
//
#include "OpenSLAudioPlay.h"
#include <pthread.h>
#include <jni.h>

//播放的pcm文件
FILE *pcmFile = 0;

OpenSLAudioPlay *slAudioPlay = nullptr;

//是否正在播放
bool isPlaying = false;

void *playThreadFunc(void *arg);

void *playThreadFunc(void *arg) {
    const int bufferSize = 2048;
    short buffer[bufferSize];
    while (isPlaying && !feof(pcmFile)) {
        fread(buffer, 1, bufferSize, pcmFile);
        slAudioPlay->enqueueSample(buffer, bufferSize);
    }
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_audiovideo_audio_AudioPlayerActivity_nativePlayPcm(JNIEnv *env, jobject thiz,
        jstring pcm_path) {
    //将java 传递过来的字符串 String 转为C 中的char *
    const char* _pcmPath = env->GetStringUTFChars(pcm_path, NULL);

    //如果已经实例化就释放资源
    if (slAudioPlay) {
        slAudioPlay->release();
        delete slAudioPlay;
        slAudioPlay = nullptr;
    }

    //实例化 OpenSLAudioPlay
    slAudioPlay = new OpenSLAudioPlay(44100, SAMPLE_FORMAT_16, 1);
    slAudioPlay->init();
    pcmFile = fopen(_pcmPath, "r");
    isPlaying = true;
    pthread_t playThread;
    pthread_create(&playThread, nullptr, playThreadFunc, 0);

    env->ReleaseStringUTFChars(pcm_path, _pcmPath);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_meitu_audiovideo_audio_AudioPlayerActivity_nativeStopPcm(JNIEnv *env, jobject thiz) {
    isPlaying = false;
    if (slAudioPlay) {
        slAudioPlay->release();
        delete slAudioPlay;
        slAudioPlay = nullptr;
    }

    if (pcmFile) {
        fclose(pcmFile);
        pcmFile = nullptr;
    }
}
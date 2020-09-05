//
// Created by jayden on 2020/8/29.
//

#ifndef NATIVEDOING_OPENSLRENDER_H
#define NATIVEDOING_OPENSLRENDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <queue>
#include <string>
#include <cstdint>
#include <thread>
#include "AudioRender.h"
#include "AudioVisualRender.h"

#define MAX_QUEUE_BUFFER_SIZE 3

/**
 * Object和Interface 是 OpenSL ES 的两大基本概念。
 *  - Interface 为每个对象提供一系列的基本功能操作
 *  - 外部只能通过GetInterface来访问Object的Interface，才能使用Object提供的功能
 *
 *
 * OpenSL ES的object有三个状态，分别是：
 * SL_OBJECT_STATE_UNREALIZED， 不可用状态时，系统不会为其分配资源；
 * SL_OBJECT_STATE_REALIZED, 当调用 realize方法后便进入此可用状态，此时对象的各个功能和资源才可用正常访问
 * SL_OBJECT_STATE_SUSPENDED，当系统音频相关硬件被其他进程占用进入此状态，调用resume后恢复到此状态
 */
class OpenSLRender : public AudioRender {
public:
    OpenSLRender(){}
    virtual ~OpenSLRender(){}
    virtual void init();
    virtual void clearAudioCache();
    virtual void renderAudioFrame(uint8_t *pData, int dataSize);
    virtual void unInit();

private:
    int createEngine();
    int createOutputMixer();
    int createAudioPlayer();
    void startRender();
    int getAudioFrameQueueSize();
    void handleAudioFrameQueue();
    static void createSLWaitingThread(OpenSLRender *openSLRender);
    static void audioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

    SLObjectItf mEngineObj = nullptr;
    SLEngineItf mEngineEngine = nullptr;
    SLObjectItf mOutputMixObj = nullptr;
    SLObjectItf mAudioPlayerObj = nullptr;
    SLPlayItf mAudioPlayerPlay = nullptr;
    SLVolumeItf mAudioPlayerVolume = nullptr;
    SLAndroidSimpleBufferQueueItf mBufferQueue;

    std::queue<AudioFrame *> mAudioFrameQueue;
    std::thread *mThread = nullptr;
    std::mutex mMutex;
    std::condition_variable mCond;
    volatile bool mExit = false;
};


#endif //NATIVEDOING_OPENSLRENDER_H

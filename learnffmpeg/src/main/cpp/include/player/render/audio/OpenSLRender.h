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

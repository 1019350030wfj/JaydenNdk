//
// Created by jayden on 2020/9/22.
//

#ifndef NATIVEDOING_OPENSLAUDIOPLAY_H
#define NATIVEDOING_OPENSLAUDIOPLAY_H

#include "AudioEngine.h"

#define SAMPLE_FORMAT_16 16

class OpenSLAudioPlay {
private:
    AudioEngine *mAudioEngine;
    SLObjectItf mPlayerObj;
    SLPlayItf mPlayer;
    SLAndroidSimpleBufferQueueItf mBufferQueue;
    SLEffectSendItf mEffectSend;
    SLVolumeItf mVolume;
    SLmilliHertz mSampleRate;
    int mSampleFormat;
    int mChannels;

    uint8_t *mBuffers[2];
    SLuint32 mBufSize;
    int mIndex;
    pthread_mutex_t mMutex;

public:
    OpenSLAudioPlay(int samplerRate, int sampleFormat, int channels);
    bool init();

    void enqueueSample(void *data, size_t length);

    void release();

    ~OpenSLAudioPlay();

    friend void playerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);
};


#endif //NATIVEDOING_OPENSLAUDIOPLAY_H

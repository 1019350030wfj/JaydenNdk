//
// Created by jayden on 2020/8/29.
//

#include "OpenSLRender.h"
#include "LogUtil.h"
#include <unistd.h>


void OpenSLRender::init() {
    LOGE("OpenSLRender::init");
    int result = -1;
    do {
       result = createEngine();
       if (result != SL_RESULT_SUCCESS) {
           LOGE("OpenSLRender::init CreateEngine fail. result=%d", result);
           break;
       }
       result = createOutputMixer();
       if (result != SL_RESULT_SUCCESS) {
           LOGE("OpenSLRender::init createOutputMixer fail.result=%", result);
           break;
       }
       result = createAudioPlayer();
       if (result != SL_RESULT_SUCCESS) {
           LOGE("OpenSLRender::init createAudioPlayer fail. result=%d", result);
           break;
       }
       mThread = new std::thread(createSLWaitingThread, this);
    } while (false);

    if (result != SL_RESULT_SUCCESS) {
        LOGE("OpenSLRender::init fail. result=%d", result);
        unInit();
    }
}

void OpenSLRender::renderAudioFrame(uint8_t *pData, int dataSize) {
    LOGE("OpenSLRender::RenderAudioFrame pData=%p, dataSize=%d", pData, dataSize);
    if (mAudioPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {
            while (getAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE && !mExit) {
                std::this_thread::sleep_for(std::chrono::microseconds(15));
            }

            std::unique_lock<std::mutex> lock(mMutex);
            AudioFrame *audioFrame = new AudioFrame(pData, dataSize);
            mAudioFrameQueue.push(audioFrame);
            mCond.notify_all();
            lock.unlock();
        }
    }
}

void OpenSLRender::unInit() {
    LOGE("OpenSLRender::unInit");
    if (mAudioPlayerPlay) {
        (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_STOPPED);
        mAudioPlayerPlay = nullptr;
    }

    std::unique_lock<std::mutex> lock(mMutex);
    mExit = true;
    mCond.notify_all();
    lock.unlock();

    if (mAudioPlayerObj) {
        (*mAudioPlayerObj)->Destroy(mAudioPlayerObj);
        mAudioPlayerObj = nullptr;
        mBufferQueue = nullptr;
    }

    if (mOutputMixObj) {
        (*mOutputMixObj)->Destroy(mOutputMixObj);
        mOutputMixObj = nullptr;
    }

    if (mEngineObj) {
        // 释放引擎对象的资源
        (*mEngineObj)->Destroy(mEngineObj);
        mEngineObj = nullptr;
        mEngineEngine = nullptr;
    }

    lock.lock();
    for (int i = 0; i < mAudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = mAudioFrameQueue.front();
        mAudioFrameQueue.pop();
        delete audioFrame;
    }
    lock.unlock();

    if (mThread != nullptr) {
        mThread->join();
        delete mThread;
        mThread = nullptr;
    }

    AudioVisualRender::releaseInstance();
}

int OpenSLRender::createEngine() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        // 创建引擎对象
        result = slCreateEngine(&mEngineObj, 0, nullptr, 0, nullptr, nullptr);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createEngine slCreateEngine fail. result=%d", result);
            break;
        }
        // 实例化
        result = (*mEngineObj)->Realize(mEngineObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createEngine Realize fail.result=%d", result);
            break;
        }

        // 获取引擎对象接口
        result = (*mEngineObj)->GetInterface(mEngineObj, SL_IID_ENGINE, &mEngineEngine);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createEngine GetInterface fail.result=%d", result);
            break;
        }
    } while (false);
    return result;
}

int OpenSLRender::createOutputMixer() {
    SLresult result = SL_RESULT_SUCCESS;
    do {
        const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
        const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

        result = (*mEngineEngine)->CreateOutputMix(mEngineEngine, &mOutputMixObj, 1, mids, mreq);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateOutputMixer createOutputMix fail. result=%d", result);
            break;
        }

        result = (*mOutputMixObj)->Realize(mOutputMixObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::CreateOutputMixer Realize fail. result=%d", result);
            break;
        }
    } while (false);
    return result;
}

int OpenSLRender::createAudioPlayer() {
    //数据与简单缓冲队列定位器
    SLDataLocator_AndroidSimpleBufferQueue androidQueue = {SL_DATALOCATOR_ANDROIDBUFFERQUEUE, 2};
    //PCM 数据源格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//格式类型
            (SLuint32)2,//通道数
            SL_SAMPLINGRATE_44_1,//采样率
            SL_PCMSAMPLEFORMAT_FIXED_16,//位宽
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//通道屏蔽
            SL_BYTEORDER_LITTLEENDIAN //字节顺序
    };

    /**
     * OpenSL ES 中的 SLDataSource 和 SLDataSink 结构体
     * 主要用于构建Audio Player 和Recorder对象
     * SLDataSource 表示音频数据来源的信息
     * SLDataSink 表示音频数据的输出信息
     */

    //数据源
    SLDataSource slDataSource = {&androidQueue, &pcm};

    // 针对数据接收器的输出混合定位器（混音器）
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, mOutputMixObj};

    //输出
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID  ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean  req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result;

    do {
        // 创建AudioPlayer 对象
        result = (*mEngineEngine)->CreateAudioPlayer(mEngineEngine, &mAudioPlayerObj, &slDataSource, &slDataSink, 3, ids, req);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createAudioPlayer CreateAudioPlayer fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->Realize(mAudioPlayerObj, SL_BOOLEAN_FALSE);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createAudioPlayer Realize fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_BUFFERQUEUE, &mBufferQueue);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createAudioPlayer GetInterface fail. result=%d", result);
            break;
        }

        result = (*mBufferQueue)->RegisterCallback(mBufferQueue, audioPlayerCallback, this);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createAudioPlayer RegisterCallback fail. result=%d", result);
            break;
        }

        result = (*mAudioPlayerObj)->GetInterface(mAudioPlayerObj, SL_IID_VOLUME, &mAudioPlayerVolume);
        if (result != SL_RESULT_SUCCESS) {
            LOGE("OpenSLRender::createAudioPlayer GetInterface volume fail. result=%d", result);
            break;
        }
    } while (false);
    return result;
}

void OpenSLRender::startRender() {
    while (getAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !mExit) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCond.wait_for(lock, std::chrono::milliseconds(10));
        lock.unlock();
    }

    (*mAudioPlayerPlay)->SetPlayState(mAudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    audioPlayerCallback(mBufferQueue, this);
}

void OpenSLRender::handleAudioFrameQueue() {
    LOGE("OpenSLRender::handleAudioFrameQueue");
    if (mAudioPlayerPlay == nullptr) {
        return;
    }

    while (getAudioFrameQueueSize() < MAX_QUEUE_BUFFER_SIZE && !mExit) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCond.wait_for(lock, std::chrono::milliseconds(10));
    }

    std::unique_lock<std::mutex> lock(mMutex);
    AudioFrame *audioFrame = mAudioFrameQueue.front();
    if (nullptr != audioFrame && mAudioPlayerPlay) {
        SLresult result = (*mBufferQueue)->Enqueue(mBufferQueue, audioFrame->data, (SLuint32) audioFrame->dataSize);
        if (result != SL_RESULT_SUCCESS) {
            AudioVisualRender::getInstance()->updateAudioFrame(audioFrame);
            mAudioFrameQueue.pop();
            delete audioFrame;
        }
    }
    lock.unlock();
}

void OpenSLRender::createSLWaitingThread(OpenSLRender *openSLRender) {
    openSLRender->startRender();
}

void OpenSLRender::audioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    OpenSLRender *openSlRender = static_cast<OpenSLRender *>(context);
    openSlRender->handleAudioFrameQueue();
}

int OpenSLRender::getAudioFrameQueueSize() {
    std::unique_lock<std::mutex> lock(mMutex);
    return mAudioFrameQueue.size();
}

void OpenSLRender::clearAudioCache() {
    std::unique_lock<std::mutex>lock(mMutex);
    for (int i = 0; i < mAudioFrameQueue.size(); ++i) {
        AudioFrame *audioFrame = mAudioFrameQueue.front();
        mAudioFrameQueue.pop();
        delete audioFrame;
    }
}




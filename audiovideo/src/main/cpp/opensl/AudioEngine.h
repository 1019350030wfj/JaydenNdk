//
// Created by jayden on 2020/9/22.
//

#ifndef NATIVEDOING_AUDIOENGINE_H
#define NATIVEDOING_AUDIOENGINE_H
// 这是标准的OpenSL ES库
#include <SLES/OpenSLES.h>
#include <stdio.h>

// 这里是针对安卓的扩展，如果要垮平台则需要注意
#include <SLES/OpenSLES_Android.h>
#include <assert.h>
#include <android/log.h>

#define LOG_TAG "wfj"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

class AudioEngine{
public:
    SLObjectItf engineObj;
    SLEngineItf engine;

    SLObjectItf outputMixObj;

    AudioEngine(): engineObj(nullptr), engine(nullptr), outputMixObj(nullptr) {
        createEngine();
    }

    virtual ~AudioEngine() {
        release();
    }

private:
    void createEngine() {
        // 音频的播放， 就涉及到 OpenSL ES
        // 第一大步： 创建引擎获取引擎接口
        // 1.1 创建引擎对象： SLObjectItf engineObj
        SLresult result = slCreateEngine(&engineObj, 0, NULL, 0, NULL, NULL);
        if (result != SL_RESULT_SUCCESS) {
            LOGD("OpenSL ES slCreateEngine fail!!!");
            return;
        }
        // 1.2 初始化引擎
        result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
        if (SL_BOOLEAN_FALSE != result) {
            LOGD("OpenSL ES Realize fail!!!");
            return;
        }
        // 1.3 获取引擎接口 SLEngineItf engineInterface
        result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
        if (result != SL_RESULT_SUCCESS) {
            LOGD("OpenSL ES GetInterface fail!!!");
            return;
        }
        // 第二大步： 设置混音器
        // 2.1 创建混音器：SLObjectItf outputMixObj
        result = (*engine)->CreateOutputMix(engine, &outputMixObj, 0, 0, 0);
        if (SL_RESULT_SUCCESS != result) {
            LOGD("OpenSL ES CreateOutputMix fail!!!");
            return;
        }
        // 2.2 初始化 混音器
        result = (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);
        if (SL_BOOLEAN_FALSE != result) {
            LOGD("OpenSL ES outputMix Realize fail!!!");
            return;
        }
    }

    virtual void release() {
        if (outputMixObj) {
            (*outputMixObj)->Destroy(outputMixObj);
            outputMixObj = nullptr;
        }

        if (engineObj) {
            (*engineObj)->Destroy(engineObj);
            engineObj = nullptr;
            engine = nullptr;
        }
    }
};

#endif //NATIVEDOING_AUDIOENGINE_H

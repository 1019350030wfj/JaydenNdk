//
// Created by jayden on 2020/8/28.
//

#ifndef NATIVEDOING_NATIVERENDER_H
#define NATIVEDOING_NATIVERENDER_H

#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#include "VideoRender.h"

class NativeRender: public VideoRender {
public:
    NativeRender(JNIEnv *env, jobject surface);
    virtual ~NativeRender();
    virtual void init(int videoWidth, int videoHeight, int *dstSize);
    virtual void renderVideoFrame(NativeImage *pImage);
    virtual void unInit();

private:
    ANativeWindow_Buffer mANativeWindowBuffer;
    ANativeWindow *mANativeWindow = nullptr;
    int mDstWidth;
    int mDstHeight;
};


#endif //NATIVEDOING_NATIVERENDER_H

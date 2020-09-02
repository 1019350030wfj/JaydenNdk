//
// Created by jayden on 2020/8/28.
//

#include "NativeRender.h"

NativeRender::NativeRender(JNIEnv *env, jobject surface) {
    mANativeWindow = ANativeWindow_fromSurface(env, surface);
}

NativeRender::~NativeRender() {
    if (mANativeWindow) {
        ANativeWindow_release(mANativeWindow);
    }
}

void NativeRender::init(int videoWidth, int videoHeight, int *dstSize) {
    LOGE("NativeRender::init mNativeWindow=%p, video[w,h]=[%d, %d]", mANativeWindow, videoWidth, videoHeight);
    if (mANativeWindow == nullptr) {
        return;
    }
    int windowWidth = ANativeWindow_getWidth(mANativeWindow);
    int windowHeight = ANativeWindow_getHeight(mANativeWindow);

    if (windowWidth < windowHeight * videoWidth / videoHeight) {
        mDstWidth = windowWidth;
        mDstHeight = windowWidth * videoHeight / videoWidth;
    } else {
        mDstWidth = windowHeight * videoWidth / videoHeight;
        mDstHeight = windowHeight;
    }

    LOGE("NativeRender::init window[w,h]=[%d, %d],DstSize[w,h]=[%d, %d]", windowWidth, windowHeight, mDstWidth, mDstHeight);

    ANativeWindow_setBuffersGeometry(mANativeWindow, mDstWidth, mDstHeight, WINDOW_FORMAT_RGBA_8888);

    dstSize[0] = mDstWidth;
    dstSize[1] = mDstHeight;
}

void NativeRender::renderVideoFrame(NativeImage *pImage) {
    if (mANativeWindow == nullptr || pImage == nullptr) {
        return;
    }
    ANativeWindow_lock(mANativeWindow, &mANativeWindowBuffer, nullptr);
    uint8_t *dstBuffer = static_cast<uint8_t *>(mANativeWindowBuffer.bits);

    int srcLineSize = pImage->width * 4;//RGBA
    int dstLineSize = mANativeWindowBuffer.stride * 4;

    for (int i = 0; i < mDstHeight; ++i) {
        memcpy(dstBuffer + i * dstLineSize, pImage->ppPlane[0] + i * srcLineSize, srcLineSize);
    }

    ANativeWindow_unlockAndPost(mANativeWindow);
}

void NativeRender::unInit() {

}

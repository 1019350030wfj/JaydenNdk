//
// Created by jayden on 2020/8/30.
//

#ifndef NATIVEDOING_VIDEODECODER_H
#define NATIVEDOING_VIDEODECODER_H

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};

#include <render/video/VideoRender.h>
#include "DecoderBase.h"

class VideoDecoder: public DecoderBase {
public:
    VideoDecoder(char *url) {
        init(url, AVMEDIA_TYPE_VIDEO);
    }

    virtual ~VideoDecoder() {
        unInit();
    }

    int getVideoHeight() {
        return mVideoHeight;
    }

    int getVideoWidth() {
        return mVideoWidth;
    }

    void setVideoRender(VideoRender *videoRender) {
        mVideoRender = videoRender;
    }

    static long getVideoDecoderTimestampForAVSync(void* context);

private:
    virtual void onDecoderReady();
    virtual void onDecoderDone();
    virtual void onFrameAvailable(AVFrame *frame);

    const AVPixelFormat DST_PIXEL_FORMAT = AV_PIX_FMT_BGRA;

    int mVideoHeight = 0;
    int mVideoWidth = 0;

    int mRenderWidth = 0;
    int mRenderHeight = 0;

    AVFrame *mRGBAFrame = nullptr;
    uint8_t *mFrameBuffer = nullptr;

    VideoRender *mVideoRender = nullptr;
    SwsContext *mSwsContext = nullptr;
};


#endif //NATIVEDOING_VIDEODECODER_H

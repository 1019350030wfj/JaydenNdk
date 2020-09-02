//
// Created by jayden on 2020/8/30.
//

#include "VideoDecoder.h"

void VideoDecoder::onDecoderReady() {
    LOGD("VideoDecoder::onDecoderReady");
    mVideoWidth = getCodecContext()->width;
    mVideoHeight = getCodecContext()->height;

    if (mMsgContext && mMsgCallback) {
        mMsgCallback(mMsgContext, MSG_DECODER_READY, 0);
    }

    if (mVideoRender != nullptr) {
        int dstSize[2] = {0};
        mVideoRender->init(mVideoWidth, mVideoHeight, dstSize);
        mRenderWidth = dstSize[0];
        mRenderHeight = dstSize[1];

        mRGBAFrame = av_frame_alloc();
        int bufferSize = av_image_get_buffer_size(DST_PIXEL_FORMAT, mRenderWidth, mRenderHeight, 1);
        mFrameBuffer = (uint8_t *) av_malloc(bufferSize * sizeof(uint8_t));
        av_image_fill_arrays(mRGBAFrame->data, mRGBAFrame->linesize,
                mFrameBuffer, DST_PIXEL_FORMAT, mRenderWidth, mRenderHeight, 1);
        mSwsContext = sws_getContext(mVideoWidth, mVideoHeight, getCodecContext()->pix_fmt,
                mRenderWidth, mRenderHeight, DST_PIXEL_FORMAT, SWS_FAST_BILINEAR, NULL, NULL, NULL);
    } else {
        LOGE("VideoRender::onDecoderReady mVideoRender == null");
    }
}

void VideoDecoder::onDecoderDone() {
    LOGD("VideoDecoder::onDecoderDone");
    if (mMsgContext && mMsgCallback) {
        mMsgCallback(mMsgContext, MSG_DECODER_DONE, 0);
    }
    if (mVideoRender) {
        mVideoRender->unInit();
    }
    if (mRGBAFrame != nullptr) {
        av_frame_free(&mRGBAFrame);
        mRGBAFrame = nullptr;
    }

    if (mFrameBuffer != nullptr) {
        free(mFrameBuffer);
        mFrameBuffer = nullptr;
    }
    if (mSwsContext != nullptr) {
        sws_freeContext(mSwsContext);
        mSwsContext = nullptr;
    }
}

void VideoDecoder::onFrameAvailable(AVFrame *frame) {
    LOGD("VideoDecoder::onFrameAvailable");
    if (mVideoRender != nullptr) {
        sws_scale(mSwsContext, frame->data, frame->linesize, 0,
                mVideoHeight, mRGBAFrame->data, mRGBAFrame->linesize);
        NativeImage image;
        image.format = IMAGE_FORMAT_RGBA;
        image.width = mRenderWidth;
        image.height = mRenderHeight;
        image.ppPlane[0] = mRGBAFrame->data[0];
        mVideoRender->renderVideoFrame(&image);
    }

    if (mMsgContext && mMsgCallback) {
        mMsgCallback(mMsgContext, MSG_REQUEST_RENDER, 0);
    }
}

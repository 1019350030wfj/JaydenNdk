//
// Created by jayden on 2020/8/30.
//

#include "AudioDecoder.h"
#include <LogUtil.h>

void AudioDecoder::onDecoderReady() {
    LOGE("AudioDecoder::OnDecoderReady");
    if (mAudioRender) {
        AVCodecContext *codeCtx = getCodecContext();

        mSwrContext = swr_alloc();

        av_opt_set_int(mSwrContext, "in_channel_layout", codeCtx->channel_layout, 0);
        av_opt_set_int(mSwrContext, "out_channel_layout", AUDIO_DST_CHANNEL_LAYOUT, 0);

        av_opt_set_int(mSwrContext, "in_sample_rate", codeCtx->sample_rate, 0);
        av_opt_set_int(mSwrContext, "out_sample_rate", AUDIO_DST_SAMPLE_RATE, 0);

        av_opt_set_sample_fmt(mSwrContext, "in_sample_fmt", codeCtx->sample_fmt, 0);
        av_opt_set_sample_fmt(mSwrContext, "out_sample_fmt", DST_SAMPLE_FORMAT, 0);

        swr_init(mSwrContext);

        LOGE("AudioDecoder::OnDecoderReady audio metadata sample rate: %d, channel: %d, format %d, frameSize %d, channelLayout %lld",
                codeCtx->sample_rate, codeCtx->channels, codeCtx->sample_fmt, codeCtx->frame_size, codeCtx->channel_layout);

        //resample params
        mNBSamples = (int) av_rescale_rnd(ACC_NB_SAMPLES, AUDIO_DST_SAMPLE_RATE, codeCtx->sample_rate, AV_ROUND_UP);
        mDstFrameDataSize = av_samples_get_buffer_size(NULL, AUDIO_DST_CHANNEL_COUNTS, mNBSamples, DST_SAMPLE_FORMAT, 1);

        LOGE("AudioDecode::OnDecoderReady [mNBSamples, mDstFrameDataSize]=[%d, %d]", mNBSamples, mDstFrameDataSize);

        mAudioOutBuffer = (uint8_t *) malloc(mDstFrameDataSize);
        mAudioRender->init();
    } else {
        LOGE("AudioDecoder::OnDecoderReady mAudioRender == null");
    }
}

void AudioDecoder::onFrameAvailable(AVFrame *frame) {
    LOGE("AudioDecoder::onFrameAvailable");
    if (mAudioRender) {
        int result = swr_convert(mSwrContext, &mAudioOutBuffer, mDstFrameDataSize / 2, (const uint8_t **) frame->data, frame->nb_samples);
        if (result > 0) {
            mAudioRender->renderAudioFrame(mAudioOutBuffer, mDstFrameDataSize);
        }
    }
}

void AudioDecoder::onDecoderDone() {
    LOGE("AudioDecoder::onDecoderDone");
    if (mAudioRender) {
        mAudioRender->unInit();
    }
    if (mAudioOutBuffer) {
        free(mAudioOutBuffer);
        mAudioOutBuffer = nullptr;
    }
    if (mSwrContext) {
        swr_free(&mSwrContext);
        mSwrContext = nullptr;
    }
}

void AudioDecoder::clearCache() {
    if (mAudioRender) {
        mAudioRender->clearAudioCache();
    }
}

long AudioDecoder::getAudioDecoderTimestampForAVSync(void *context) {
    if (context != nullptr) {
        AudioDecoder* audioDecoder = static_cast<AudioDecoder *>(context);
        return audioDecoder->getCurrentPosition();
    }
    return 0;
}
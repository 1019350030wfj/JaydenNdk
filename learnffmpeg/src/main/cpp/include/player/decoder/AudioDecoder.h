//
// Created by jayden on 2020/8/30.
//

#ifndef NATIVEDOING_AUDIODECODER_H
#define NATIVEDOING_AUDIODECODER_H

extern "C" {
#include <libavutil/samplefmt.h>
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/audio_fifo.h>
};

#include <render/audio/AudioRender.h>
#include <Decoder.h>
#include <DecoderBase.h>

//音频编码采样率
static const int AUDIO_DST_SAMPLE_RATE = 44100;
//音频编码通道数
static const int AUDIO_DST_CHANNEL_COUNTS = 2;
//音频编码声道格式
static const uint64_t AUDIO_DST_CHANNEL_LAYOUT = AV_CH_LAYOUT_STEREO;
//音频编码比特率
static const int AUDIO_DST_BIT_RATE = 64000;
//ACC音频一帧采样数
static const int ACC_NB_SAMPLES = 1024;

class AudioDecoder: public DecoderBase {
public:
    AudioDecoder(char *url) {
        init(url, AVMEDIA_TYPE_AUDIO);
    }

    virtual ~AudioDecoder() {
        unInit();
    }

    void setVideoRender(AudioRender *audioRender) {
        mAudioRender = audioRender;
    }

    static long getAudioDecoderTimestampForAVSync(void *context);

private:
    virtual void onDecoderReady();
    virtual void onDecoderDone();
    virtual void onFrameAvailable(AVFrame *frame);
    virtual void clearCache();

    const AVSampleFormat DST_SAMPLE_FORMAT = AV_SAMPLE_FMT_S16;

    AudioRender *mAudioRender = nullptr;
    SwrContext *mSwrContext = nullptr;
    uint8_t *mAudioOutBuffer = nullptr;

    int mNBSamples= 0;
    int mDstFrameDataSize = 0;
};


#endif //NATIVEDOING_AUDIODECODER_H

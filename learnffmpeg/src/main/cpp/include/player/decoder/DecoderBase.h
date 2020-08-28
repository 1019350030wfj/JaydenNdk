//
// Created by jayden on 2020/8/26.
//

#ifndef NATIVEDOING_DECODERBASE_H
#define NATIVEDOING_DECODERBASE_H

extern "C" {
#include <libavutil/time.h>
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
};

#include <thread>
#include "Decoder.h"

#define MAX_PATH 2048

using namespace std;

enum DecoderState {
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP
};

enum DecoderMsg {
    MSG_DECODER_INIT_ERROR,
    MSG_DECODER_READY,
    MSG_DECODER_DONE,
    MSG_REQUEST_RENDER,
    MSG_DECODING_TIME
};

class DecoderBase: public Decoder {
public:
    DecoderBase(){};
    virtual ~DecoderBase(){};

    virtual void start();
    virtual void pause();
    virtual void stop();
    virtual float getDuration() {
        //ms to s
        return mDuration * 1.f / 1000;
    }
    virtual void seekToPosition(float position);
    virtual float getCurrentPosition();
    virtual void clearCache(){};
    virtual void setMessageCallback(void* context, MessageCallback callback) {
        mMsgContext = context;
        mMsgCallback = callback;
    }

    virtual void setAVSyncCallback(void* context, AVSyncCallback callback) {
        mAVDecoderContext = context;
        mAudioSyncCallback = callback;
    }

protected:
    void *mMsgContext = nullptr;
    MessageCallback mMsgCallback = nullptr;
    virtual int Init(const char *url, AVMediaType mediaType);
    virtual void unInit();
    virtual void onDecoderReady() = 0;
    virtual void onDecoderDone() = 0;
    virtual void onFrameAvailable(AVFrame *frame) = 0;

    AVCodecContext *getCodecContext() {
        return mAVCodecContext;
    }
private:
    int initFFDecoder();
    void unInitDecoder();

    void startDecodingThread();
    void decodingLoop();
    void updaetTimeStamp();
    void avSync();
    int decodeOnePacket();
    static void doAvDecoding(DecoderBase *decoder);

    //封装格式上下文
    AVFormatContext *mAVFormatContext = nullptr;
    //解码器上下文
    AVCodecContext *mAVCodecContext = nullptr;
    //解码器
    AVCodec *mAVCodec = nullptr;
    //编码的数据包
    AVPacket *mPacket = nullptr;
    //解码的帧
    AVFrame *mFrame = nullptr;
    //数据流的类型
    AVMediaType mMediaType = AVMEDIA_TYPE_UNKNOWN;
    //文件地址
    char mUrl[MAX_PATH] = {0};
    //当前播放时间
    long mCurTimeStamp = 0;
    //播放的起始时间
    long mStartTimeStamp = -1;
    //总时长
    long mDuration = 0;
    //数据流索引
    int mStreamIndex = -1;
    //锁和条件变量
    mutex mMutex;
    condition_variable mCond;
    thread *mThread = nullptr;
    //seek position
    volatile float mSeekPosition = 0;
    volatile bool mSeekSuccess = false;
    //解码器状态
    volatile int mDecoderState = STATE_UNKNOWN;
    void *mAVDecoderContext = nullptr;
    AVSyncCallback mAudioSyncCallback = nullptr;//用作音视频同步

};


#endif //NATIVEDOING_DECODERBASE_H

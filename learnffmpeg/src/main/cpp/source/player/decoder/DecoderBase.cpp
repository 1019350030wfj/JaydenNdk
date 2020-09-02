//
// Created by jayden on 2020/8/26.
//

#include "DecoderBase.h"
#include "LogUtil.h"

int DecoderBase::init(const char *url, AVMediaType mediaType) {
    LOGE("DecoderBase::init url=%s, mediaType=%d", url, mediaType);
    strcpy(mUrl, url);
    mMediaType = mediaType;
    return 0;
}

void DecoderBase::unInit() {
    LOGE("DecoderBase::unInit mMediaType=%d", mMediaType);
    if (mThread) {
        stop();
        mThread->join();
        delete mThread;
        mThread = nullptr;
    }
    LOGE("DecoderBase::unInit end mMediaType=%d", mMediaType);
}

void DecoderBase::start() {
    if (mThread == nullptr) {
        startDecodingThread();
    } else {
        std::unique_lock<std::mutex> lock(mMutex);
        mDecoderState = STATE_DECODING;
        mCond.notify_all();
    }
}

void DecoderBase::pause() {
    std::unique_lock<std::mutex>(mMutex);
    mDecoderState = STATE_PAUSE;
}

void DecoderBase::stop() {
    LOGD("DecoderBase::Stop");
    std::unique_lock<std::mutex> lock(mMutex);
    mDecoderState = STATE_STOP;
    mCond.notify_all();
}

void DecoderBase::seekToPosition(float position) {
    std::unique_lock<std::mutex>(mMutex);
    mSeekPosition = position;
    mDecoderState = STATE_DECODING;
    mCond.notify_all();
}

float DecoderBase::getCurrentPosition() {
    std::unique_lock<std::mutex>(mMutex);
    return mCurTimeStamp;
}

int DecoderBase::initFFDecoder() {
    int result = -1;
    do {
        //1.创建封装格式上下文
        mAVFormatContext = avformat_alloc_context();

        //2.打开文件
        if(avformat_open_input(&mAVFormatContext, mUrl, NULL, NULL) != 0)
        {
            LOGE("DecoderBase::InitFFDecoder avformat_open_input fail.");
            break;
        }

        //3.获取音视频流信息
        if(avformat_find_stream_info(mAVFormatContext, NULL) < 0) {
            LOGE("DecoderBase::InitFFDecoder avformat_find_streaminfo fail.");
            break;
        }

        //4.获取音视频流索引
        for(int i=0; i < mAVFormatContext->nb_streams; i++) {
            if(mAVFormatContext->streams[i]->codecpar->codec_type == mMediaType) {
                mStreamIndex = i;
                break;
            }
        }

        if(mStreamIndex == -1) {
            LOGE("DecoderBase::InitFFDecoder Fail to find stream index.");
            break;
        }
        //5.获取解码器参数
        AVCodecParameters *codecParameters = mAVFormatContext->streams[mStreamIndex]->codecpar;

        //6.获取解码器
        mAVCodec = avcodec_find_decoder(codecParameters->codec_id);
        if(mAVCodec == nullptr) {
            LOGE("DecoderBase::InitFFDecoder avcodec_find_decoder fail.");
            break;
        }

        //7.创建解码器上下文
        mAVCodecContext = avcodec_alloc_context3(mAVCodec);
        if(avcodec_parameters_to_context(mAVCodecContext, codecParameters) != 0) {
            LOGE("DecoderBase::InitFFDecoder avcodec_parameters_to_context fail.");
            break;
        }

        //8.打开解码器
        result = avcodec_open2(mAVCodecContext, mAVCodec, NULL);
        if(result < 0) {
            LOGE("DecoderBase::InitFFDecoder avcodec_open2 fail. result=%d", result);
            break;
        }
        result = 0;

        mDuration = mAVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms
        //创建 AVPacket 存放编码数据
        mPacket = av_packet_alloc();
        //创建 AVFrame 存放解码后的数据
        mFrame = av_frame_alloc();
    } while (false);

    if(result != 0 && mMsgContext && mMsgCallback)
        mMsgCallback(mMsgContext, MSG_DECODER_INIT_ERROR, 0);

    return result;
}

void DecoderBase::unInitDecoder() {
    LOGE("DecoderBase::UnInitDecoder");
    if(mFrame != nullptr) {
        av_frame_free(&mFrame);
        mFrame = nullptr;
    }

    if(mPacket != nullptr) {
        av_packet_free(&mPacket);
        mPacket = nullptr;
    }

    if(mAVCodecContext != nullptr) {
        avcodec_close(mAVCodecContext);
        avcodec_free_context(&mAVCodecContext);
        mAVCodecContext = nullptr;
        mAVCodec = nullptr;
    }

    if(mAVFormatContext != nullptr) {
        avformat_close_input(&mAVFormatContext);
        avformat_free_context(mAVFormatContext);
        mAVFormatContext = nullptr;
    }

}

void DecoderBase::startDecodingThread() {
    mThread = new thread(doAvDecoding, this);
}

void DecoderBase::decodingLoop() {
    LOGE("DecoderBase::DecodingLoop start, mMediaType=%d", mMediaType);
    {
        std::unique_lock<std::mutex> lock(mMutex);
        mDecoderState = STATE_DECODING;
        lock.unlock();
    }

    for(;;) {

        while (mDecoderState == STATE_PAUSE) {
            std::unique_lock<std::mutex> lock(mMutex);
            LOGE("DecoderBase::DecodingLoop waiting, mMediaType=%d", mMediaType);
            mCond.wait_for(lock, std::chrono::milliseconds(10));
            mStartTimeStamp = GetSysCurrentTime() - mCurTimeStamp;
        }

        if(mDecoderState == STATE_STOP) {
            break;
        }

        if(mStartTimeStamp == -1)
            mStartTimeStamp = GetSysCurrentTime();

        if(decodeOnePacket() != 0) {
            //解码结束，暂停解码器
            std::unique_lock<std::mutex> lock(mMutex);
            mDecoderState = STATE_PAUSE;
        }
    }

    LOGE("DecoderBase::DecodingLoop end");

}

void DecoderBase::updateTimeStamp() {
    LOGE("DecoderBase::UpdateTimeStamp");
    std::unique_lock<std::mutex> lock(mMutex);
    if(mFrame->pkt_dts != AV_NOPTS_VALUE) {
        mCurTimeStamp = mFrame->pkt_dts;
    } else if (mFrame->pts != AV_NOPTS_VALUE) {
        mCurTimeStamp = mFrame->pts;
    } else {
        mCurTimeStamp = 0;
    }

    mCurTimeStamp = (int64_t)((mCurTimeStamp * av_q2d(mAVFormatContext->streams[mStreamIndex]->time_base)) * 1000);

    if(mSeekPosition > 0 && mSeekSuccess)
    {
        mStartTimeStamp = GetSysCurrentTime() - mCurTimeStamp;
        mSeekPosition = 0;
        mSeekSuccess = false;
    }
}

void DecoderBase::avSync() {
    LOGE("DecoderBase::avSync");
    long curSysTime = GetSysCurrentTime();
    //基于系统时钟计算从开始播放流逝的时间
    long elapsedTime = curSysTime - mStartTimeStamp;

    if(mMsgContext && mMsgCallback && mMediaType == AVMEDIA_TYPE_VIDEO)
        mMsgCallback(mMsgContext, MSG_DECODING_TIME, mCurTimeStamp * 1.0f / 1000);

    //向系统时钟同步
//    if(mCurTimeStamp > elapsedTime) {
//        //休眠时间
//        auto sleepTime = static_cast<unsigned int>(mCurTimeStamp - elapsedTime);//ms
//        av_usleep(sleepTime * 1000);
//    }

    if(mAudioSyncCallback != nullptr) {
        //视频向音频同步,或者音频向视频同步,视传进来的 mAudioSyncCallback 而定
        long elapsedTime = mAudioSyncCallback(mAVDecoderContext);
        LOGE("DecoderBase::AVSync mCurTimeStamp=%ld, elapsedTime=%ld", mCurTimeStamp, elapsedTime);

        if(mCurTimeStamp > elapsedTime) {
            //休眠时间
            auto sleepTime = static_cast<unsigned int>(mCurTimeStamp - elapsedTime);//ms
            av_usleep(sleepTime * 1000);
        }
    }
}

int DecoderBase::decodeOnePacket() {
    LOGE("DecoderBase::DecodeOnePacket mMediaType=%d", mMediaType);
    if(mSeekPosition > 0) {
        //seek to frame
        int64_t seek_target = static_cast<int64_t>(mSeekPosition * 1000000);//微秒
        int64_t seek_min = INT64_MIN;
        int64_t seek_max = INT64_MAX;
        int seek_ret = avformat_seek_file(mAVFormatContext, -1, seek_min, seek_target, seek_max, 0);
        if (seek_ret < 0) {
            mSeekSuccess = false;
            LOGE("BaseDecoder::DecodeOneFrame error while seeking");
        } else {
            if (-1 != mStreamIndex) {
                avcodec_flush_buffers(mAVCodecContext);
            }
            clearCache();
            mSeekSuccess = true;
            LOGE("BaseDecoder::DecodeOneFrame seekFrame pos=%f", mSeekPosition);
        }
    }
    int result = av_read_frame(mAVFormatContext, mPacket);
    while(result == 0) {
        if(mPacket->streamindex == mStreamIndex) {
            if(avcodec_send_packet(mAVCodecContext, mPacket) == AVERROR_EOF) {
                //解码结束
                result = -1;
                goto __EXIT;
            }

            //一个 packet 包含多少 frame?
            int frameCount = 0;
            while (avcodec_receive_frame(mAVCodecContext, mFrame) == 0) {
                //更新时间戳
                updateTimeStamp();
                //同步
                avSync();
                //渲染
                LOGE("DecoderBase::DecodeOnePacket 000 mMediaType=%d", mMediaType);
                onFrameAvailable(mFrame);
                LOGE("DecoderBase::DecodeOnePacket 0001 mMediaType=%d", mMediaType);
                frameCount ++;
            }
            LOGE("BaseDecoder::DecodeOneFrame frameCount=%d", frameCount);
            //判断一个 packet 是否解码完成
            if(frameCount > 0) {
                result = 0;
                goto __EXIT;
            }
        }
        av_packet_unref(mPacket);
        result = av_read_frame(mAVFormatContext, mPacket);
    }

    __EXIT:
    av_packet_unref(mPacket);
    return result;
}

void DecoderBase::doAvDecoding(DecoderBase *decoder) {
    LOGE("DecoderBase::DoAVDecoding");
    do {
        if(decoder->initFFDecoder() != 0) {
            break;
        }
        decoder->onDecoderReady();
        decoder->decodingLoop();
    } while (false);

    decoder->unInitDecoder();
    decoder->onDecoderDone();

}


//
// Created by jayden on 2020/8/26.
//

#include "DecoderBase.h"
#include "LogUtil.h"

int DecoderBase::Init(const char *url, AVMediaType mediaType) {
    return 0;
}

void DecoderBase::unInit() {

}

void DecoderBase::start() {

}

void DecoderBase::pause() {

}

void DecoderBase::stop() {
    LOGD("DecoderBase::Stop");
}

void DecoderBase::seekToPosition(float position) {

}

float DecoderBase::getCurrentPosition() {
    return 0;
}


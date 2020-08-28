//
// Created by jayden on 2020/8/26.
//

#ifndef NATIVEDOING_DECODER_H
#define NATIVEDOING_DECODER_H

//函数指针 是指向函数的指针
//指针函数 是返回值是指针的函数
typedef void (*MessageCallback)(void*, int ,float);
typedef long (*AVSyncCallback)(void*);

/**
 * 虚基类不能定义对象
 */
class Decoder {
public:
    virtual void start() = 0;//纯虚函数，派生类必须重写
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual float getDuration() = 0;
    virtual void seekToPosition(float position) = 0;
    virtual float getCurrentPosition() = 0;
    virtual void setMessageCallback(void* context, MessageCallback callback) = 0;
};

#endif //NATIVEDOING_DECODER_H

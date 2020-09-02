//
// Created by jayden on 2020/8/28.
//

#ifndef NATIVEDOING_AUDIORENDER_H
#define NATIVEDOING_AUDIORENDER_H

class AudioFrame {
public:
    AudioFrame(uint8_t *data, int dataSize, bool hardCopy = true) {
        this->dataSize = dataSize;
        this->data = data;
        this->hardCopy = hardCopy;
        if (hardCopy) {
            this->data = static_cast<uint8_t *>(malloc(this->dataSize));
            memcpy(this->data, data, dataSize);
        }
    }
    ~AudioFrame() {
        if (hardCopy && this->data) {
            free(this->data);
        }
        this->data = nullptr;
    }

    int dataSize = 0;
    uint8_t *data = nullptr;
    bool hardCopy = true;
};

class AudioRender {
public:
    virtual ~AudioRender() {}
    virtual void init() = 0;
    virtual void clearAudioCache() = 0;
    virtual void renderAudioFrame(uint8_t *pData, int dataSize) = 0;
    virtual void unInit() = 0;
};

#endif //NATIVEDOING_AUDIORENDER_H

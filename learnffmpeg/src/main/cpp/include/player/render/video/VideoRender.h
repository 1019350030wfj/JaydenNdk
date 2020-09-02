//
// Created by jayden on 2020/8/27.
//

#ifndef NATIVEDOING_VIDEORENDER_H
#define NATIVEDOING_VIDEORENDER_H

#include "ImageDef.h"

class VideoRender {
public:
    virtual ~VideoRender(){}
    virtual void init(int videoWidth, int videoHeight, int *dstSize) = 0;
    virtual void renderVideoFrame(NativeImage *pImage) = 0;
    virtual void unInit() = 0;
};

#endif //NATIVEDOING_VIDEORENDER_H

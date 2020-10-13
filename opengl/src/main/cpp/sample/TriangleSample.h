//
// Created by jayden on 2020/10/12.
//

#ifndef NATIVEDOING_TRIANGLESAMPLE_H
#define NATIVEDOING_TRIANGLESAMPLE_H


#include "GLSampleBase.h"

class TriangleSample : public GLSampleBase{
public:
    TriangleSample();
    virtual ~TriangleSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void Init();

    virtual void Draw(int screenW, int screenH);

    virtual void Destroy();
};


#endif //NATIVEDOING_TRIANGLESAMPLE_H

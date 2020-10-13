//
// Created by jayden on 2020/10/12.
//

#ifndef NATIVEDOING_MYGLRENDERCONTEXT_H
#define NATIVEDOING_MYGLRENDERCONTEXT_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include "TriangleSample.h"

class MyGLRenderContext {
    MyGLRenderContext();

    ~MyGLRenderContext();

public:
    void onDrawFrame();

    void onSurfaceChanged(int width, int height);

    void onSurfaceCreated();

    static MyGLRenderContext* GetInstance();

    static void DestroyInstance();


private:
    static MyGLRenderContext *m_pContext;
    int m_ScreenW;
    int m_ScreenH;
    GLSampleBase *m_pBeforeSample;
    GLSampleBase *m_pCurSample;
};


#endif //NATIVEDOING_MYGLRENDERCONTEXT_H

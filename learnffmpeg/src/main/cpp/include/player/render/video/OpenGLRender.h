//
// Created by jayden on 2020/8/31.
//

#ifndef NATIVEDOING_OPENGLRENDER_H
#define NATIVEDOING_OPENGLRENDER_H

#include <thread>
#include <ImageDef.h>
#include <VideoRender.h>
#include <GLES3/gl3.h>
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <vec2.hpp>

using namespace glm;

#define MATH_PI 3.1415926535897932384626433832802

class OpenGLRender: public VideoRender {
public:
    virtual void init(int videoWidth, int videoHeight, int *dstSize);
    virtual void renderVideoFrame(NativeImage *pImage);
    virtual void unInit();

    void onSurfaceCreated();
    void onSurfaceChanged(int w, int h);
    void onDrawFrame();

    static OpenGLRender *getInstance();
    static void releaseInstance();

    void updateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY);

    void setTouchLoc(float touchX, float touchY) {
        mTouchXY.x = touchX / mScreenSize.x;
        mTouchXY.y = touchY / mScreenSize.y;
    }

private:
    OpenGLRender();
    virtual ~OpenGLRender();

    static std::mutex mMutex;
    static OpenGLRender* sInstance;
    GLuint mProgramObj = GL_NONE;
    GLuint mTextureId;
    GLuint mVaoId;
    GLuint mVboIds[3];
    NativeImage mRenderImage;
    glm::mat4 mMVPMatrix;

    int mFrameIndex;
    vec2 mTouchXY;
    vec2 mScreenSize;
};


#endif //NATIVEDOING_OPENGLRENDER_H

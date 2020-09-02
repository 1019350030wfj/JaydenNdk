//
// Created by jayden on 2020/8/29.
//

#ifndef NATIVEDOING_AUDIOVISUALRENDER_H
#define NATIVEDOING_AUDIOVISUALRENDER_H

#include <thread>
#include <GLES3/gl3.h>
#include "AudioRender.h"
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>

using namespace glm;

const int MAX_AUDIO_LEVEL = 5000;
#define RESAMPLE_LEVEL 40


class AudioVisualRender {
public:
    static AudioVisualRender* getInstance();
    static void releaseInstance();

    void onAudioVisualSurfaceCreated();
    void onAudioVisualSurfaceChanged(int w, int h);
    void onAudioVisualDrawFrame();

    void updateAudioFrame(AudioFrame *audioFrame);

private:
    void init();
    void unInit();
    AudioVisualRender() {
        init();
    }
    ~AudioVisualRender() {
        unInit();
    }

    void updateMesh();

    static AudioVisualRender *mPInstance;
    static std::mutex mMutex;
    AudioFrame *mPAudioBuffer = nullptr;

    GLuint mProgramObj;
    GLuint mVaoId;
    GLuint mVboIds[2];
    glm::mat4 mMVPMatrix;

    vec3 *mPVerticesCoords = nullptr;
    vec2 *mPTextureCoords = nullptr;

    int mRenderDataSize;
};


#endif //NATIVEDOING_AUDIOVISUALRENDER_H

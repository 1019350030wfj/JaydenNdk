//
// Created by jayden on 2020/8/29.
//

#include <GLUtils.h>
#include "AudioVisualRender.h"
#include <LogUtil.h>
#include <render/video/OpenGLRender.h>
#include <detail/type_mat.hpp>
#include <detail/type_mat4x4.hpp>
#include <gtc/matrix_transform.hpp>

AudioVisualRender* AudioVisualRender::mPInstance = nullptr;
std::mutex AudioVisualRender::mMutex;

AudioVisualRender* AudioVisualRender::getInstance() {
    if (mPInstance == nullptr) {
        std::unique_lock<std::mutex> lock(mMutex);
        if (mPInstance == nullptr) {
            mPInstance = new AudioVisualRender();
        }
    }
    return mPInstance;
}

void AudioVisualRender::releaseInstance() {
    std::unique_lock<std::mutex> lock(mMutex);
    if (mPInstance != nullptr) {
        delete mPInstance;
        mPInstance = nullptr;
    }
}
void AudioVisualRender::onAudioVisualSurfaceCreated() {
    LOGE("AudioVisualRender::OnAudioVisualSurfaceCreated");
    if (mProgramObj)
        return;
    char vShaderStr[] =
            "#version 300 es\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    v_texCoord = a_texCoord;\n"
            "    gl_PointSize = 4.0f;\n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform float drawType;                             \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  if(drawType == 1.0)                               \n"
            "  {                                                 \n"
            "      outColor = vec4(1.5 - v_texCoord.y, 0.3, 0.3, 1.0); \n"
            "  }                                                 \n"
            "  else if(drawType == 2.0)                          \n"
            "  {                                                 \n"
            "      outColor = vec4(1.0, 1.0, 1.0, 1.0);          \n"
            "  }                                                 \n"
            "  else if(drawType == 3.0)                          \n"
            "  {                                                 \n"
            "      outColor = vec4(0.3, 0.3, 0.3, 1.0);          \n"
            "  }                                                 \n"
            "}                                                   \n";

    mProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if (mProgramObj == GL_NONE) {
        LOGE("VisualizeAudioSample::Init create program fail");
    }


    //设置 MVP Matrix
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(1.0f, 1.0f, 1.0f));
    Model = glm::rotate(Model, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mMVPMatrix = Projection * View * Model;
}

void AudioVisualRender::onAudioVisualSurfaceChanged(int w, int h) {
    LOGE("AudioVisualRender::OnAudioVisualSurfaceChanged [w, h] = [%d, %d]", w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0);
    glViewport(0, 0, w, h);
}

void AudioVisualRender::onAudioVisualDrawFrame() {
    LOGE("AudioVisualRender::OnAudioVisualDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    std::unique_lock<std::mutex> lock(mMutex);
    if (mProgramObj == GL_NONE || mPAudioBuffer == nullptr) return;
    updateMesh();
    lock.unlock();

    // Generate VBO Ids and load the VBOs with data
    if(mVboIds[0] == 0)
    {
        glGenBuffers(2, mVboIds);

        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mRenderDataSize * 6 * 3, mPVerticesCoords, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mRenderDataSize * 6 * 2, mPTextureCoords, GL_DYNAMIC_DRAW);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * mRenderDataSize * 6 * 3, mPVerticesCoords);

        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * mRenderDataSize * 6 * 2, mPTextureCoords);
    }

    if(mVaoId == GL_NONE)
    {
        glGenVertexArrays(1, &mVaoId);
        glBindVertexArray(mVaoId);

        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        glBindBuffer(GL_ARRAY_BUFFER, mVboIds[1]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        glBindVertexArray(GL_NONE);
    }

    // Use the program object
    glUseProgram(mProgramObj);
    glBindVertexArray(mVaoId);
    GLUtils::setMat4(mProgramObj, "u_MVPMatrix", mMVPMatrix);
    GLUtils::setFloat(mProgramObj, "drawType", 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, mRenderDataSize * 6);
    GLUtils::setFloat(mProgramObj, "drawType", 2.0f);
    glDrawArrays(GL_LINES, 0, mRenderDataSize * 6);

}

void AudioVisualRender::updateAudioFrame(AudioFrame *audioFrame) {
    if(audioFrame != nullptr) {
        LOGD("AudioVisualRender::UpdateAudioFrame audioFrame->dataSize=%d", audioFrame->dataSize);
        std::unique_lock<std::mutex> lock(mMutex);
        if(mPAudioBuffer != nullptr && mPAudioBuffer->dataSize != audioFrame->dataSize) {
            delete mPAudioBuffer;
            mPAudioBuffer = nullptr;

            delete [] mPTextureCoords;
            mPTextureCoords = nullptr;

            delete [] mPVerticesCoords;
            mPVerticesCoords = nullptr;
        }

        if(mPAudioBuffer == nullptr) {
            mPAudioBuffer = new AudioFrame(audioFrame->data, audioFrame->dataSize);
            mRenderDataSize = mPAudioBuffer->dataSize / RESAMPLE_LEVEL;

            mPVerticesCoords = new vec3[mRenderDataSize * 6]; //(x,y,z) * 6 points
            mPTextureCoords = new vec2[mRenderDataSize * 6]; //(x,y) * 6 points
        } else {
            memcpy(mPAudioBuffer->data, audioFrame->data, audioFrame->dataSize);
        }
        lock.unlock();
    }
}

void AudioVisualRender::updateMesh() {
    float dy = 0.25f / MAX_AUDIO_LEVEL;
    float dx = 1.0f / mRenderDataSize;
    for (int i = 0; i < mRenderDataSize; ++i) {
        int index = i * RESAMPLE_LEVEL;
        short *pValue = (short *)(mPAudioBuffer->data + index);
        float y = *pValue * dy;
        y = y < 0 ? y : -y;
        vec2 p1(i * dx, 0 + 1.0f);
        vec2 p2(i * dx, y + 1.0f);
        vec2 p3((i + 1) * dx, y + 1.0f);
        vec2 p4((i + 1) * dx, 0 + 1.0f);

        mPTextureCoords[i * 6 + 0] = p1;
        mPTextureCoords[i * 6 + 1] = p2;
        mPTextureCoords[i * 6 + 2] = p4;
        mPTextureCoords[i * 6 + 3] = p4;
        mPTextureCoords[i * 6 + 4] = p2;
        mPTextureCoords[i * 6 + 5] = p3;

        mPVerticesCoords[i * 6 + 0] = GLUtils::texCoordToVertexCoord(p1);
        mPVerticesCoords[i * 6 + 1] = GLUtils::texCoordToVertexCoord(p2);
        mPVerticesCoords[i * 6 + 2] = GLUtils::texCoordToVertexCoord(p4);
        mPVerticesCoords[i * 6 + 3] = GLUtils::texCoordToVertexCoord(p4);
        mPVerticesCoords[i * 6 + 4] = GLUtils::texCoordToVertexCoord(p2);
        mPVerticesCoords[i * 6 + 5] = GLUtils::texCoordToVertexCoord(p3);
    }
}

void AudioVisualRender::init() {
    mVaoId = GL_NONE;

    mPTextureCoords = nullptr;
    mPVerticesCoords = nullptr;

    memset(mVboIds, 0, sizeof(GLuint) * 2);
    mPAudioBuffer = nullptr;
}

void AudioVisualRender::unInit() {
    if (mPAudioBuffer != nullptr) {
        delete mPAudioBuffer;
        mPAudioBuffer = nullptr;
    }

    if (mPTextureCoords != nullptr) {
        delete [] mPTextureCoords;
        mPTextureCoords = nullptr;
    }

    if (mPVerticesCoords != nullptr) {
        delete [] mPVerticesCoords;
        mPVerticesCoords = nullptr;
    }
}
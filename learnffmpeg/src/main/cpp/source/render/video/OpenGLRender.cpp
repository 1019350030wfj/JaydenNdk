//
// Created by jayden on 2020/8/31.
//

#include "OpenGLRender.h"
#include <GLUtils.h>
#include <gtc/matrix_transform.hpp>

OpenGLRender* OpenGLRender::sInstance = nullptr;
std::mutex OpenGLRender::mMutex;

static char vShaderStr[] =
        "#version 300 es\n"
        "layout(location = 0) in vec4 a_position;\n"
        "layout(location = 1) in vec2 a_texCoord;\n"
        "uniform mat4 u_MVPMatrix;\n"
        "out vec2 v_texCoord;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = u_MVPMatrix * a_position;\n"
        "    v_texCoord = a_texCoord;\n"
        "}";

static char fShaderStr[] =
        "//dynimic mesh 动态网格\n"
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TextureMap;//采样器\n"
        "uniform float u_Offset;\n"
        "uniform vec2 u_TexSize;\n"
        "void main()\n"
        "{\n"
        "    vec2 imgTexCoord = v_texCoord * u_TexSize;\n"
        "    float sideLength = u_TexSize.y / 6.0;\n"
        "    float maxOffset = 0.15 * sideLength;\n"
        "    float x = mod(imgTexCoord.x, floor(sideLength));\n"
        "    float y = mod(imgTexCoord.y, floor(sideLength));\n"
        "\n"
        "    float offset = u_Offset * maxOffset;\n"
        "\n"
        "    if(offset <= x\n"
        "    && x <= sideLength - offset\n"
        "    && offset <= y\n"
        "    && y <= sideLength - offset)\n"
        "    {\n"
        "        outColor = texture(s_TextureMap, v_texCoord);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    }\n"
        "}";

static char fGrayShaderStr[] =
        "//黑白滤镜\n"
        "#version 300 es\n"
        "precision highp float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TextureMap;//采样器\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(s_TextureMap, v_texCoord);\n"
        "    if(v_texCoord.x > 0.5)\n"
        "        outColor = vec4(vec3(outColor.r*0.299 + outColor.g*0.587 + outColor.b*0.114), outColor.a);\n"
        "}";

GLfloat verticesCoords[] = {
        -1.0f,  1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f,  -1.0f, 0.0f,  // Position 2
        1.0f,   1.0f, 0.0f,  // Position 3
};

GLfloat textureCoords[] = {
        0.0f,  0.0f,        // TexCoord 0
        0.0f,  1.0f,        // TexCoord 1
        1.0f,  1.0f,        // TexCoord 2
        1.0f,  0.0f         // TexCoord 3
};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

OpenGLRender::OpenGLRender() {

}

OpenGLRender::~OpenGLRender() {
    NativeImageUtil::freeNativeImage(&mRenderImage);
}

void OpenGLRender::init(int videoWidth, int videoHeight, int *dstSize) {
    LOGE("OpenGLRender::InitRender video[w, h]=[%d, %d]", videoWidth, videoHeight);
    std::unique_lock<std::mutex> lock(mMutex);
    mRenderImage.format = IMAGE_FORMAT_RGBA;
    mRenderImage.width = videoWidth;
    mRenderImage.height = videoHeight;

    dstSize[0] = videoWidth;
    dstSize[1] = videoHeight;
    mFrameIndex = 0;
}

void OpenGLRender::renderVideoFrame(NativeImage *pImage) {
    LOGE("OpenGLRender::renderVideoFrame pImage=%p", pImage);
    if(pImage == nullptr || pImage->ppPlane[0] == nullptr)
        return;
    std::unique_lock<std::mutex> lock(mMutex);
    if(mRenderImage.ppPlane[0] == nullptr)
    {
        NativeImageUtil::allocNativeImage(&mRenderImage);
    }

    NativeImageUtil::copyNativeImage(pImage, &mRenderImage);
}

void OpenGLRender::unInit() {

}

void OpenGLRender::updateMVPMatrix(int angleX, int angleY, float scaleX, float scaleY)
{
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mMVPMatrix = Projection * View * Model;

}

void OpenGLRender::onSurfaceCreated() {
    LOGE("OpenGLRender::OnSurfaceCreated");

    mProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr);
    if (!mProgramObj)
    {
        LOGE("OpenGLRender::OnSurfaceCreated create program fail");
        return;
    }

    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, mVboIds);
    glBindBuffer(GL_ARRAY_BUFFER, mVboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, mVboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &mVaoId);
    glBindVertexArray(mVaoId);

    glBindBuffer(GL_ARRAY_BUFFER, mVboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, mVboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *)0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVboIds[2]);

    glBindVertexArray(GL_NONE);

    updateMVPMatrix(0, 0, 1.0f, 1.0f);
    mTouchXY = vec2(0.5f, 0.5f);
}

void OpenGLRender::onSurfaceChanged(int w, int h) {
    LOGE("OpenGLRender::OnSurfaceChanged [w, h]=[%d, %d]", w, h);
    mScreenSize.x = w;
    mScreenSize.y = h;
    glViewport(0, 0, w, h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGLRender::onDrawFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    if(mProgramObj == GL_NONE || mTextureId == GL_NONE || mRenderImage.ppPlane[0] == nullptr) return;
    LOGE("OpenGLRender::OnDrawFrame [w, h]=[%d, %d]", mRenderImage.width, mRenderImage.height);
    mFrameIndex++;

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    std::unique_lock<std::mutex> lock(mMutex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mRenderImage.width, mRenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[0]);
    lock.unlock();

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Use the program object
    glUseProgram (mProgramObj);

    glBindVertexArray(mVaoId);

    GLUtils::setMat4(mProgramObj, "u_MVPMatrix", mMVPMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTextureId);
    GLUtils::setFloat(mProgramObj, "s_TextureMap", 0);

    //float time = static_cast<float>(fmod(mFrameIndex, 60) / 50);
    //GLUtils::setFloat(mProgramObj, "u_Time", time);

    float offset = (sin(mFrameIndex * MATH_PI / 25) + 1.0f) / 2.0f;
    GLUtils::setFloat(mProgramObj, "u_Offset", offset);
    GLUtils::setVec2(mProgramObj, "u_TexSize", vec2(mRenderImage.width, mRenderImage.height));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);

}

OpenGLRender* OpenGLRender::getInstance() {
    if (sInstance == nullptr) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (sInstance == nullptr) {
            sInstance = new OpenGLRender();
        }
    }
}

void OpenGLRender::releaseInstance() {
    if (sInstance != nullptr) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (sInstance != nullptr) {
            delete sInstance;
            sInstance = nullptr;
        }
    }
}
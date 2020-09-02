//
// Created by jayden on 2020/8/25.
//

#include <render/video/NativeRender.h>
#include <render/audio/OpenSLRender.h>
#include <render/video/OpenGLRender.h>
#include "FFMediaPlayer.h"

void FFMediaPlayer::Init(JNIEnv *jniEnv, jobject obj, char* url, int renderType, jobject surface) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);
    mVideoDecoder = new VideoDecoder(url);
    mAudioDecoder = new AudioDecoder(url);

    if (renderType == VIDEO_RENDER_OPENGL) {
        mVideoDecoder->setVideoRender(OpenGLRender::getInstance());
    } else if (renderType == VIDEO_RENDER_ANWINDOW) {
        mVideoRender = new NativeRender(jniEnv, surface);
        mVideoDecoder->setVideoRender(mVideoRender);
    }

    mAudioRender = new OpenSLRender();
    mAudioDecoder->setVideoRender(mAudioRender);

    mAudioDecoder->setMessageCallback(this, postMessage);
    mVideoDecoder->setMessageCallback(this, postMessage);

    mVideoDecoder->setAVSyncCallback(mAudioDecoder, AudioDecoder::getAudioDecoderTimestampForAVSync);
}

void FFMediaPlayer::UnInit() {
    if (mVideoDecoder) {
        delete mVideoDecoder;
        mVideoDecoder = nullptr;
    }

    if (mVideoRender) {
        delete mVideoRender;
        mVideoRender = nullptr;
    }

    if (mAudioDecoder) {
        delete mAudioDecoder;
        mAudioDecoder = nullptr;
    }

    if (mAudioRender) {
        delete mAudioRender;
        mAudioRender = nullptr;
    }

    OpenGLRender::releaseInstance();
    bool isAttach = false;
    getJNIEnv(&isAttach)->DeleteGlobalRef(m_JavaObj);
    if (isAttach) {
        getJavaVM()->DetachCurrentThread();
    }
}

void FFMediaPlayer::play() {
    LOGE("FFMediaPlayer::play");
    if (mVideoDecoder) {
        mVideoDecoder->start();
    }

    if (mAudioDecoder) {
        mAudioDecoder->start();
    }
}

void FFMediaPlayer::pause() {
    LOGE("FFMediaPlayer::pause");
    if (mVideoDecoder) {
        mVideoDecoder->pause();
    }
    if (mAudioDecoder) {
        mAudioDecoder->pause();
    }
}

void FFMediaPlayer::stop() {
    LOGE("FFMediaPlayer::stop");
    if (mVideoDecoder) {
        mVideoDecoder->stop();
    }
    if (mAudioDecoder) {
        mAudioDecoder->stop();
    }
}

void FFMediaPlayer::seekToPosition(float position) {
    if (mVideoDecoder) {
        mVideoDecoder->seekToPosition(position);
    }
    if (mAudioDecoder) {
        mAudioDecoder->seekToPosition(position);
    }
}

long FFMediaPlayer::getMediaParams(int paramType) {
    long value = 0;
    switch (paramType) {
        case MEDIA_PARAM_VIDEO_WIDTH: {
            value = mVideoDecoder != nullptr ? mVideoDecoder->getVideoWidth() : 0;
            break;
        }
        case MEDIA_PARAM_VIDEO_HEIGHT: {
            value = mVideoDecoder != nullptr ? mVideoDecoder->getVideoHeight() : 0;
            break;
        }
        case MEDIA_PARAM_VIDEO_DURATION: {
            value = mVideoDecoder != nullptr ? mVideoDecoder->getDuration() : 0;
            break;
        }
    }
    return value;
}

JNIEnv* FFMediaPlayer::getJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == m_JavaVM) {
        LOGE("FFMediaPlayer::GetJNIEnv m_JavaVM == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = m_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = m_JavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGE("FFMediaPlayer::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}

jobject FFMediaPlayer::getJavaObj() {
    return m_JavaObj;
}

JavaVM* FFMediaPlayer::getJavaVM() {
    return m_JavaVM;
}

void FFMediaPlayer::postMessage(void *context, int msgType, float msgCode) {
    if (context != nullptr) {
        FFMediaPlayer *player = static_cast<FFMediaPlayer *>(context);
        bool isAttach = false;
        JNIEnv *env = player->getJNIEnv(&isAttach);
        LOGE("FFMediaPlayer::PostMessage env=%p", env);
        if (env == nullptr) {
            return;
        }
        jobject javaObj = player->getJavaObj();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj), JAVA_PLAYER_EVENT_CALLBACK_API_NAME, "(IF)V");
        env->CallVoidMethod(javaObj, mid, msgType, msgCode);
        if (isAttach) {
            player->getJavaVM()->DetachCurrentThread();
        }
    }
}
//
// Created by jayden on 2020/8/21.
//

#include <cstdio>
#include <cstring>
#include "jni.h"
#include "LogUtil.h"
#include "FFMediaPlayer.h"

extern "C" {
#include <libavcodec/version.h>
#include <libavfilter/version.h>
#include <libavformat/version.h>
#include <libavutil/version.h>
#include <libswresample/version.h>
#include <libswscale/version.h>
#include <libavcodec/avcodec.h>
};

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jlong JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1Init(JNIEnv *env, jobject thiz, jstring jurl,
                                                            jint video_render_type,
                                                            jobject surface) {
    //从String -> jstring -> char *
    const char *url = env->GetStringUTFChars(jurl, nullptr);
    FFMediaPlayer *player = new FFMediaPlayer();
    player->Init(env, thiz, const_cast<char *>(url), video_render_type, surface);
    env->ReleaseStringUTFChars(jurl, url);
    return reinterpret_cast<jlong>(player);//将指针对象转为地址，然后给上层
}

JNIEXPORT void JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1Play(JNIEnv *env, jobject thiz,
                                                            jlong player_handle) {
    LOGD("FFMediaPlayer::Play");
    if (player_handle != 0) {
       FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *> (player_handle);
       ffMediaPlayer->play();
    }
}

JNIEXPORT void JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1Pause(JNIEnv *env, jobject thiz,
                                                             jlong player_handle) {
    if (player_handle != 0) {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(player_handle);
        ffMediaPlayer->pause();
    }
}

JNIEXPORT void JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1unInit(JNIEnv *env, jobject thiz,
                                                              jlong player_handle) {
    if (player_handle != 0) {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *> (player_handle);
        ffMediaPlayer->UnInit();
    }

}

JNIEXPORT jlong JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1GetMediaParams(JNIEnv *env, jobject thiz,
                                                                      jlong m_native_player_handle,
                                                                      jint param_type) {
    long value = 0;
    if (m_native_player_handle != 0) {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *> (m_native_player_handle);
        value = ffMediaPlayer->getMediaParams(param_type);
    }
    return value;
}

JNIEXPORT void JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1SeekToPosition(JNIEnv *env, jobject thiz,
                                                                      jlong m_native_player_handle,
                                                                      jfloat progress) {
    if (m_native_player_handle != 0) {
        FFMediaPlayer *ffMediaPlayer = reinterpret_cast<FFMediaPlayer *>(m_native_player_handle);
        ffMediaPlayer->seekToPosition(progress);
    }
}

JNIEXPORT jstring JNICALL
Java_com_meitu_learnffmpeg_media_FFMediaPlayer_native_1GetFFmpegVersion(JNIEnv *env, jclass clazz) {
    char strBuffer[1024 * 4] = {0};
    strcat(strBuffer, "libavcodec: ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVCODEC_VERSION));
    strcat(strBuffer, "\n libavfilter: ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFILTER_VERSION));
    strcat(strBuffer, "\n libavformat: ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVFORMAT_VERSION));
    strcat(strBuffer, "\n libavutil: ");
    strcat(strBuffer, AV_STRINGIFY(LIBAVUTIL_VERSION));
    strcat(strBuffer, "\n libswresample: ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWRESAMPLE_VERSION));
    strcat(strBuffer, "\n libswscale: ");
    strcat(strBuffer, AV_STRINGIFY(LIBSWSCALE_VERSION));
    strcat(strBuffer, "\n avcodec_configure: \n");
    strcat(strBuffer, avcodec_configuration());
    strcat(strBuffer, "\n avcodec_license \n");
    strcat(strBuffer, avcodec_license());
    LOGI("GetFFmpegVersion \n%s", strBuffer);
    return env->NewStringUTF(strBuffer);
}
#ifdef __cplusplus
};
#endif

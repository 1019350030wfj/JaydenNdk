//
// Created by jayden on 2020/8/21.
//

#ifndef NATIVEDOING_LOGUTIL_H
#define NATIVEDOING_LOGUTIL_H

#include "../../../../../../../../Library/Android/sdk/ndk/16.1.4479499/sysroot/usr/include/android/log.h"

#define LOG_TAG "Jayden"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


#endif //NATIVEDOING_LOGUTIL_H

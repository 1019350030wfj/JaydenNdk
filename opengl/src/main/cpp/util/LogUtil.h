//
// Created by jayden on 2020/10/10.
//

#ifndef NATIVEDOING_LOGUTIL_H
#define NATIVEDOING_LOGUTIL_H

#include <android/log.h>

#define LOG_TAG "jayden"
#define LOGCATE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //NATIVEDOING_LOGUTIL_H

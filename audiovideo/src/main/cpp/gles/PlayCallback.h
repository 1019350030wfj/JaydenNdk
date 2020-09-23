//
// Created by jayden on 2020/9/23.
//

#ifndef NATIVEDOING_PLAYCALLBACK_H
#define NATIVEDOING_PLAYCALLBACK_H

#include <jni.h>

class PlayCallback {
public:
    PlayCallback(JavaVM *javaVm, JNIEnv *env, jobject job);
    ~PlayCallback();

    void onSucceed(const char *);
    void onError(const char *);

    void toJavaMessage(const char *message);

private:
    JavaVM *javaVm = 0;
    JNIEnv *env = 0;
    jobject instance;
    jmethodID jmd_showMessage;
};


#endif //NATIVEDOING_PLAYCALLBACK_H

//
// Created by jayden on 2020/9/23.
//

#include "PlayCallback.h"

PlayCallback::PlayCallback(JavaVM *javaVm, JNIEnv *env, jobject job) {
    this->javaVm = javaVm;
    this->env = env;
    this->instance = env->NewGlobalRef(job);//提升全局
}

PlayCallback::~PlayCallback() {

}

void PlayCallback::toJavaMessage(const char *message) {
    //拿到jclass
    jclass videoPlayClass = this->env->GetObjectClass(instance);
    //拿到methodID
    this->jmd_showMessage = this->env->GetMethodID(videoPlayClass, "showMessage", "(Ljava/lang/String;)V");
    jstring string = env->NewStringUTF(message);
    //通过反射执行Java方法
    this->env->CallVoidMethod(instance, jmd_showMessage, string);
}

void PlayCallback::onSucceed(const char *msg) {
    toJavaMessage(msg);
}

void PlayCallback::onError(const char * message) {
    toJavaMessage(message);
}

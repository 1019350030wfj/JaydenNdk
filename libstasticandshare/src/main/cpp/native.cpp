//
// Created by jayden on 2020/9/3.
//

#include <jni.h>
#include <string>
#include <jayden.h>
#include <yangyang.h>

extern "C"{
JNIEXPORT jstring JNICALL
Java_com_meitu_libstasticandshare_MainActivity_getStringFromJNI(JNIEnv* env, jobject obj) {
    char age[3];
    sprintf(age, "%d", yangYangFun.getYangYangKey());
    std::string value = yangYangFun.getResponse() + " I am " + age + " years old.\n";
    value += jaydenFun.getResponse();

    return env->NewStringUTF(value.c_str());
}
}

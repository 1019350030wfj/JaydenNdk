//
// Created by jayden on 2020/10/10.
//

#include <jni.h>
#include "util/LogUtil.h"
#include <MyGLRenderContext.h>

#define NATIVE_RENDER_CLASS_NAME "com/meitu/opengl/jni/MyNativeRender"

#ifdef __cplusplus
extern "C" {
#endif
JNIEXPORT void JNICALL native_init(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance();
}

JNIEXPORT void JNICALL native_UnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::DestroyInstance();
}

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance()->onDrawFrame();
}

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance()->onSurfaceCreated();
}

JNIEXPORT void JNICALL
native_OnSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height) {
    MyGLRenderContext::GetInstance()->onSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL
native_SetParamsInt(JNIEnv *env, jobject instance, jint paramType, jint value0, jint value1) {

}

JNIEXPORT void JNICALL
native_SetParamsFloat(JNIEnv *env, jobject instance, jint type, jfloat x, jfloat y) {

}

JNIEXPORT void JNICALL
native_SetImageData(JNIEnv *env, jobject instance, jint format, jint width, jint height,
                    jbyteArray bytes) {

}

JNIEXPORT void JNICALL
native_SetImageDataWithIndex(JNIEnv *env, jobject instance, jint index, jint format, jint width,
                             jint height, jbyteArray bytes) {

}

JNIEXPORT void JNICALL native_SetAudioData(JNIEnv *env, jobject instance, jshortArray audioData) {

}

JNIEXPORT void JNICALL
native_UpdateTransformMatrix(JNIEnv *env, jobject instance, jfloat rotateX, jfloat rotateY,
                             jfloat scaleX, jfloat scaleY) {

}

#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
        {"native_init",                  "()V",       (void *) (native_init)},
        {"native_UnInit",                "()V",       (void *) (native_UnInit)},
        {"native_OnDrawFrame",           "()V",       (void *) (native_OnDrawFrame)},
        {"native_OnSurfaceChanged",      "(II)V",     (void *) (native_OnSurfaceChanged)},
        {"native_OnSurfaceCreated",      "()V",       (void *) (native_OnSurfaceCreated)},
        {"native_SetParamsInt",          "(III)V",    (void *) (native_SetParamsInt)},
        {"native_SetParamsFloat",        "(IFF)V",    (void *) (native_SetParamsFloat)},
        {"native_SetImageData",          "(III[B)V",  (void *) (native_SetImageData)},
        {"native_SetImageDataWithIndex", "(IIII[B)V", (void *) (native_SetImageDataWithIndex)},
        {"native_SetAudioData",          "([S)V",     (void *) (native_SetAudioData)},
        {"native_UpdateTransformMatrix", "(FFFF)V",   (void *) (native_UpdateTransformMatrix)},
};

static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    LOGCATE("UnregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("UnregisterNativeMethods fail. clazz == NULL");
        return;
    }
    if (env != NULL) {
        env->UnregisterNatives(clazz);
    }
}

static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGCATE("RegisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == NULL) {
        LOGCATE("RegisterNativeMethods fail. clazz == NULL");
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, methods, methodNum) < 0) {
        LOGCATE("RegisterNativeMethods fail");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

extern "C" jint JNI_OnLoad(JavaVM *javaVm, void *pVoid) {
    LOGCATE("JNI_OnLoad");
    jint jniRet = JNI_ERR;
    JNIEnv *env = NULL;
    if (javaVm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }
    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
                                        sizeof(g_RenderMethods) / sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *pVoid) {
    LOGCATE("JNI_OnUnload");
    JNIEnv *env = NULL;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }

    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);
}


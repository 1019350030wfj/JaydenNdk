#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <pthread.h>
#include <inttypes.h>
#include <assert.h>

static const char* KTAG = "hello-jniCallback";

#define LOGI(...) \
  ((void)__android_log_print(ANDROID_LOG_INFO, KTAG, __VA_ARGS__))
#define LOGW(...) \
  ((void)__android_log_print(ANDROID_LOG_WARM, KTAG, __VA_ARGS__))
#define LOGE(...) \
  ((void)__android_log_print(ANDROID_LOG_ERROR, KTAG, __VA_ARGS__))

typedef struct tick_context {
      JavaVM  *javaVm;
      jclass jniHelperClz;
      jobject jniHelperObj;
      jclass mainActivityClz;
      jobject mainActivityObj;
      pthread_mutex_t lock;
      int done;
} TickContext;
TickContext g_ctx;

void sendJavaMsg(JNIEnv *env, jobject instance, jmethodID func, const char* msg) {
    jstring javaMsg = (*env)->NewStringUTF(env, msg);
    (*env)->CallVoidMethod(env, instance, func, javaMsg);
    (*env)->DeleteLocalRef(env, javaMsg);
}

void* UpdateTicks(void* context) {
    TickContext *pctx = (TickContext*) context;
    JavaVM *javaVm = pctx->javaVm;
    JNIEnv *env;
    jint res = (*javaVm)->GetEnv(javaVm, (void**)&env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = (*javaVm)->AttachCurrentThread(javaVm, &env, NULL);
        if (res != JNI_OK) {
            LOGE("Failed to attach current thread, ErrorCode=%d", res);
            return NULL;
        }
    }

    jmethodID statusId = (*env)->GetMethodID(env, pctx->jniHelperClz, "updateStatus", "(Ljava/lang/String;)V");
    sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: initializing...");

    jmethodID timerId = (*env)->GetMethodID(env, pctx->mainActivityClz, "updateTimer", "()V");
    struct timeval beginTime, curTime, usedTime, leftTime;
    const struct timeval kOneSecond = {
            (__kernel_time_t)1,
            (__kernel_suseconds_t)0
    };
    sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: start ticking");
    while (1) {
        gettimeofday(&beginTime, NULL);
        LOGI("begin time sec: %d", beginTime.tv_sec);
        LOGI("begin time usec: %d", beginTime.tv_usec);
        pthread_mutex_lock(&pctx->lock);
        int done = pctx->done;
        if (pctx->done) {
            pctx->done = 0;
        }
        pthread_mutex_unlock(&pctx->lock);
        if (done) {
            break;
        }
        (*env)->CallVoidMethod(env, pctx->mainActivityObj, timerId);
        gettimeofday(&curTime, NULL);

        LOGI("curTime time sec: %d", curTime.tv_sec);
        LOGI("curTime time usec: %d", curTime.tv_usec);
        timersub(&curTime, &beginTime, &usedTime);

        LOGI("usedTime time sec: %d", usedTime.tv_sec);
        LOGI("usedTime time usec: %d", usedTime.tv_usec);
        LOGI("kOneSecond time sec: %d", kOneSecond.tv_sec);
        LOGI("kOneSecond time usec: %d", kOneSecond.tv_usec);

        timersub(&kOneSecond, &usedTime, &leftTime);

        LOGI("leftTime time sec: %d", leftTime.tv_sec);
        LOGI("leftTime time usec: %d", leftTime.tv_usec);
        struct timespec sleepTime;
        sleepTime.tv_sec = leftTime.tv_sec;
        sleepTime.tv_nsec = leftTime.tv_usec * 1000;
        if (sleepTime.tv_sec <= 1) {
            nanosleep(&sleepTime, NULL);
        } else {
            sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread error: processing too long");
        }
    }
    sendJavaMsg(env, pctx->jniHelperObj, statusId, "TickerThread status: ticking stopped");
    (*javaVm)->DetachCurrentThread(javaVm);
    return context;
}

JNIEXPORT jstring JNICALL
Java_com_meitu_jnicallback_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz) {
    return (*env)->NewStringUTF(env, "Hello This is From JNI!");
}

JNIEXPORT void JNICALL
Java_com_meitu_jnicallback_MainActivity_startTicks(JNIEnv* env, jobject thiz) {
    pthread_t threadInfo_;
    pthread_attr_t threadAttr_;

    pthread_attr_init(&threadAttr_);
    pthread_attr_setdetachstate(&threadAttr_, PTHREAD_CREATE_DETACHED);

    pthread_mutex_init(&g_ctx.lock, NULL);

    jclass clz = (*env)->GetObjectClass(env, thiz);
    g_ctx.mainActivityClz = (*env)->NewGlobalRef(env, clz);
    g_ctx.mainActivityObj = (*env)->NewGlobalRef(env, thiz);

    int result = pthread_create(&threadInfo_, &threadAttr_, UpdateTicks, &g_ctx);
    assert(result == 0);

    pthread_attr_destroy(&threadAttr_);

    (void)result;
}

JNIEXPORT void JNICALL
Java_com_meitu_jnicallback_MainActivity_stopTicks(JNIEnv* env, jobject thiz) {
    pthread_mutex_lock(&g_ctx.lock);
    g_ctx.done = 1;
    pthread_mutex_lock(&g_ctx.lock);

    struct timespec sleepTime;
    memset(&sleepTime, 0, sizeof(sleepTime));
    sleepTime.tv_nsec = 100000000;
    while (g_ctx.done) {
        nanosleep(&sleepTime, NULL);
    }
    (*env)->DeleteGlobalRef(env, g_ctx.mainActivityClz);
    (*env)->DeleteGlobalRef(env, g_ctx.mainActivityObj);
    g_ctx.mainActivityObj = NULL;
    g_ctx.mainActivityClz = NULL;

    pthread_mutex_destroy(&g_ctx.lock);
}

JNIEXPORT void queryRuntimeInfo(JNIEnv *env, jobject instance) {
    jmethodID versionFunc = (*env)->GetStaticMethodID(env, g_ctx.jniHelperClz, "getBuildVersion", "()Ljava/lang/String;");
    if (!versionFunc) {
        LOGE("Failed to retrieve getBuildVersion() methodID @ line %d", __LINE__);
        return;
    }
    jstring buildVersion = (*env)->CallStaticObjectMethod(env, g_ctx.jniHelperClz, versionFunc);
    const char *version = (*env)->GetStringUTFChars(env, buildVersion, NULL);
    if (!version) {
        LOGE("unable to get version string @ line %d", __LINE__);
        return;
    }
    LOGI("Android Version - %s", version);
    (*env)->ReleaseStringUTFChars(env, buildVersion, version);
    (*env)->DeleteLocalRef(env, buildVersion);

    jmethodID runtimeMemSize = (*env)->GetMethodID(env, g_ctx.jniHelperClz, "getRuntimeMemorySize", "()J");
    if (!runtimeMemSize) {
        LOGE("Failed to retrieve getRuntimeMemorySize() methodID @ line %d", __LINE__);
        return;
    }
    jlong result = (*env)->CallLongMethod(env, instance, runtimeMemSize);
    LOGI("Runtime free memory size: %" PRId64, result);
    (void)result;
}

jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;
    memset(&g_ctx, 0, sizeof(g_ctx));
    g_ctx.javaVm = vm;
    if ((*vm) -> GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;//不支持JNI版本
    }
    jclass clz = (*env) -> FindClass(env, "com/meitu/jnicallback/JniHandler1");
    g_ctx.jniHelperClz = (*env)->NewGlobalRef(env, clz);
    jmethodID jniHelperCtor = (*env)->GetMethodID(env, g_ctx.jniHelperClz, "<init>", "()V");
    jobject  handler = (*env)->NewObject(env, g_ctx.jniHelperClz, jniHelperCtor);
    g_ctx.jniHelperObj = (*env)->NewGlobalRef(env, handler);
    queryRuntimeInfo(env, g_ctx.jniHelperObj);
    g_ctx.done = 0;
    g_ctx.mainActivityObj = NULL;
    return JNI_VERSION_1_6;
}
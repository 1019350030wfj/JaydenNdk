//
// Created by jayden on 2020/9/4.
//

#include "LearnPThread.h"

void *run(void *arg) {
    //获取传入当前线程的参数
    char *threadTag = (char *) arg;
    for (int i = 0; i < 5; ++i) {
        LOGE("%s thread %d", threadTag, i);
        if (i == 4) {
            // 结束当前线程， 参数为线程结束后的返回值
            pthread_exit(threadTag);
        }
    }
    // 线程正常执行完成后的返回值
    return 0;
}

void LearnPThread::createThreads() {
    LOGE("Main Thread");
    char tagArr[][5] = {"NO.1", "NO.2", "NO.3", "NO.4", "NO.5"};

    //线程id 用于区分线程 一个线程对应一个tid
    pthread_t tids[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        //创建线程，指定run 方法， 传入参数 tagArr[i]
        pthread_create(&tids[i], NULL, run, (void *) tagArr[i]);
    }

    void *returnValue[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        //阻塞当前线程， 等待指定tid的线程结束，并获取线程返回值
        pthread_join(tids[i], &returnValue[i]);
        LOGE("thread %s terminated", (char *) (returnValue[i]));
    }
}

int globalCount = 0;
//互斥锁
pthread_mutex_t mutex;

/**
 * 模拟两个线程同时进入此方法
 */
void *asynRun(void *arg) {
    LOGE("thread %s enter", (char *) arg);
    // lock
    pthread_mutex_lock(&mutex);
    //取传入当前线程的参数
    char *threadArg = (char *) arg;

    for (int i = 0; i < 10; ++i) {
        //休眠 200ms
        usleep(200 * 1000);
        globalCount++;
        LOGE("%s thread %d, globalCount= %d",threadArg, i, globalCount);
    }

    //unlock
    pthread_mutex_unlock(&mutex);

    // 线程正常执行完成后的返回值
    return threadArg;
}

//线程同步
void LearnPThread::mutexThreads() {
    LOGE("Main Thread");
    //初始化互斥锁
    pthread_mutex_init(&mutex, NULL);
    pthread_t t1, t2;

    //创建两个线程
    pthread_create(&t1, NULL, asynRun, (void *) "NO.1");
    pthread_create(&t2, NULL, asynRun, (void *) "NO.2");

    void *returnValue[2];
    pthread_join(t1, &returnValue[0]);
    pthread_join(t2, &returnValue[1]);

    LOGE("thread %s terminated", (char *) returnValue[0]);
    LOGE("thread %s terminated", (char *) returnValue[1]);

    //销毁互斥锁
    pthread_mutex_destroy(&mutex);
}
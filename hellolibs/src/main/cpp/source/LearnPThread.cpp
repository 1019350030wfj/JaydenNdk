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
        LOGE("thread %s terminated", (char *)(returnValue[i]));
    }
}
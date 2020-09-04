//
// Created by jayden on 2020/9/4.

//线程创建相关 API：
//pthread_t 线程 id 。
//pthread_create 负责创建线程，传入 pthread_t（线程 ID ）的指针，线程的执行方法和传入线程的参数。
//pthread_join 使当前线程挂起，等待指定线程执行结束，并获取线程返回值。
//pthread_exit 退出当前线程，并且可以设置当前线程的返回值。
//pthread_cancel 终止当前线程。

#ifndef NATIVEDOING_LEARNPTHREAD_H
#define NATIVEDOING_LEARNPTHREAD_H

#include <stdlib.h>
#include <unistd.h>
#include "LogUtil.h"
#include <pthread.h>

#define NUM_THREADS 5

class LearnPThread {
public:
    void createThreads();
    void mutexThreads();
};


#endif //NATIVEDOING_LEARNPTHREAD_H

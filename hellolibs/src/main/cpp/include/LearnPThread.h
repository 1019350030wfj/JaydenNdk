//
// Created by jayden on 2020/9/4.

//线程创建相关 API：
//pthread_t 线程 id 。
//pthread_create 负责创建线程，传入 pthread_t（线程 ID ）的指针，线程的执行方法和传入线程的参数。
//pthread_join 使当前线程挂起，等待指定线程执行结束，并获取线程返回值。
//pthread_exit 退出当前线程，并且可以设置当前线程的返回值。
//pthread_cancel 终止当前线程。

/**
 * 线程同步相关 API :
pthread_mutex_t 互斥锁。
pthread_mutex_init 初始化互斥锁，需传入互斥锁的指针。
pthread_mutex_destroy 销毁互斥锁，需传入互斥锁的指针。
pthread_mutex_lock 加锁，需传入互斥锁的指针。
pthread_mutex_unlock 解锁，需传入互斥锁的指针
 */

/**
 * 线程间通信相关 API ：
 * pthread_cond_t 条件变量，条件变量是线程同步的一种手段，使线程可以休眠等待某种条件出现。
pthread_cond_signal 发送一个信号给另外一个正在处于阻塞等待状态的线程，原本这两个线程竞争同一个 mutex lock 。
pthread_cond_wait 使当前线程处于阻塞状态，直到接收到其他线程发送对应的 cond_signal 。
pthread_cond_init 初始化条件变量。
pthread_cond_destroy 销毁条件变量。
 */

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
    void threadCommunication();
};


#endif //NATIVEDOING_LEARNPTHREAD_H

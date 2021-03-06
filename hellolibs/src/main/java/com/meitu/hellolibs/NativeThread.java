package com.meitu.hellolibs;

/**
 * native 线程
 * Author: wfj
 * Date: 2020/9/4 9:28 PM
 */
public class NativeThread {

    public void createThread() {
        nativeCreateThread();
    }

    public void mutexThread() {
        nativeMutexThread();
    }

    public void threadCommunication() {
        nativeThreadCommunication();
    }

    private native void nativeCreateThread();
    private native void nativeMutexThread();
    private native void nativeThreadCommunication();

}

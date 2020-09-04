package com.meitu.hellolibs;

import android.util.Log;

/**
 * java 与 Native 交互
 * Author: wfj
 * Date: 2020/9/4 7:29 PM
 */
public class NativeAndJava {
    private int jayden = 1;

    static int sJayden = 2;

    public void nativeCallJava(String msg) {
        Log.d("wfj", "nativeCallJava msg:" + msg);
    }

    public static String nativeCallJavaStaticMethod(String msg) {
        Log.d("wfj", "nativeCallJavaStaticMethod msg:" + msg);
        return "java to native";
    }

    public native void javaCallNative(String msg);
}

package com.meitu.opengl.jni;

/**
 * 中间层 JNI方法封装
 * Author: wfj
 * Date: 2020/9/30 9:21 AM
 */
public class MyNativeRender {
    public static final int SAMPLE_TYPE  =  200;

    public static final int SAMPLE_TYPE_SET_TOUCH_LOC           = SAMPLE_TYPE + 999;
    public static final int SAMPLE_TYPE_SET_GRAVITY_XY          = SAMPLE_TYPE + 1000;

    static {
        System.loadLibrary("native-render");
    }

    public native void native_init();
    public native void native_UnInit();
    public native void native_OnDrawFrame();
    public native void native_OnSurfaceChanged(int width, int height);
    public native void native_OnSurfaceCreated();
    public native void native_SetParamsInt(int paramType, int value0, int value1);
    public native void native_SetParamsFloat(int type, float x, float y);
    public native void native_SetImageData(int format, int width, int height, byte[] bytes);
    public native void native_SetImageDataWithIndex(int index, int format, int width, int height, byte[] bytes);
    public native void native_SetAudioData(short[] audioData);
    public native void native_UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
}

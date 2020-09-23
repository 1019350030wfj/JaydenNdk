package com.meitu.audiovideo.video;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Handler;
import android.os.Looper;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.Toast;

/**
 * Author: wfj
 * Date: 2020/9/23 7:50 AM
 */
public class YUVPlayer extends GLSurfaceView implements SurfaceHolder.Callback {
    private Context context;
    private String yuv420pPath;
    private Object surface;

    public YUVPlayer(Context context) {
        super(context);
        this.context = context.getApplicationContext();
    }

    public YUVPlayer(Context context, AttributeSet attrs) {
        super(context, attrs);
        this.context = context.getApplicationContext();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        super.surfaceCreated(holder);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        super.surfaceChanged(holder, format, w, h);
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        super.surfaceDestroyed(holder);
    }

    public void glesPlay(final String yuv420pPath, final Object surface) {
        this.yuv420pPath = yuv420pPath;
        this.surface = surface;

        Thread thread = new Thread(playRunnable);
        thread.start();
    }

    private Runnable playRunnable = new Runnable() {
        @Override
        public void run() {
            nativeGlesPlay(yuv420pPath, surface);
        }
    };

    public native void nativeGlesPlay(String yuv420pPath, Object surface);
    private native void nativeWindowPlay(String yuv420pPath, Object surface);
    private native void nativeDestory();

    public void start(String yuv420pPath, Object surface) {
        nativeWindowPlay(yuv420pPath, surface);
    }

    public void stop() {
        nativeDestory();
    }

    /**
     * JNI 调用
     * @param message
     */
    public void showMessage(final String message) {
        Log.d("wfj", message);

        new Handler(Looper.getMainLooper()).post(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
            }
        });
    }
}

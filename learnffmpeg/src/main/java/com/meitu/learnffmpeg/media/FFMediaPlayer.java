package com.meitu.learnffmpeg.media;

import android.view.Surface;

public class FFMediaPlayer {
    static {
        System.loadLibrary("learn-ffmpeg");
    }
    public static final int VIDEO_RENDER_ANWINDOW = 1;
    private EventCallback mEventCallback = null;
    private long mNativePlayerHandle = 0;

    public static String getFFmpegVersion() {
        return native_GetFFmpegVersion();
    }

    public void addEventCallback(EventCallback mEventCallback) {
        this.mEventCallback = mEventCallback;
    }

    public void init(String url, int videoRenderType, Surface surface) {
        mNativePlayerHandle = native_Init(url, videoRenderType, surface);
    }

    public void play() {
        native_Play(mNativePlayerHandle);
    }

    public void pause() {
        native_Pause(mNativePlayerHandle);
    }

    public void unInit() {
        native_unInit(mNativePlayerHandle);
    }

    private void playerEventCallback(int msgType, float msgValue) {
        if (mEventCallback != null) {
            mEventCallback.onPlayerEvent(msgType, msgValue);
        }
    }

    private native long native_Init(String url, int videoRenderType, Surface surface);
    private native void native_Play(long playerHandle);
    private native void native_Pause(long playerHandle);
    private native void native_unInit(long playerHandle);

    private static native String native_GetFFmpegVersion();

    public interface EventCallback {
        void onPlayerEvent(int msgType, float msgValue);
    }
}

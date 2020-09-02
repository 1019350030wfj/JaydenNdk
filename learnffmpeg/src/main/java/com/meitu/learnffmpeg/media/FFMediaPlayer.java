package com.meitu.learnffmpeg.media;

import android.view.Surface;

public class FFMediaPlayer {
    static {
        System.loadLibrary("learn-ffmpeg");
    }

    public static final int MSG_DECODER_INIT_ERROR = 0;
    public static final int MSG_DECODER_READY = 1;
    public static final int MSG_DECODER_DONE = 2;
    public static final int MSG_REQUEST_RENDER = 3;
    public static final int MSG_DECODING_TIME = 4;

    public static final int VIDEO_RENDER_OPENGL = 0;
    public static final int VIDEO_RENDER_ANWINDOW = 1;

    public static final int MEDIA_PARAM_VIDEO_WIDTH = 0x0001;
    public static final int MEDIA_PARAM_VIDEO_HEIGHT = 0x0002;
    public static final int MEDIA_PARAM_VIDEO_DURATION = 0x0003;

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

    public long getMediaParams(int paramType) {
        return native_GetMediaParams(mNativePlayerHandle, paramType);
    }

    private void playerEventCallback(int msgType, float msgValue) {
        if (mEventCallback != null) {
            mEventCallback.onPlayerEvent(msgType, msgValue);
        }
    }

    private native long native_GetMediaParams(long mNativePlayerHandle, int paramType);
    private native void native_SeekToPosition(long mNativePlayerHandle, float progress);
    private native long native_Init(String url, int videoRenderType, Surface surface);
    private native void native_Play(long playerHandle);
    private native void native_Pause(long playerHandle);
    private native void native_unInit(long playerHandle);

    private static native String native_GetFFmpegVersion();

    public void seekToPosition(float progress) {
        native_SeekToPosition(mNativePlayerHandle, progress);
    }


    public interface EventCallback {
        void onPlayerEvent(int msgType, float msgValue);
    }
}

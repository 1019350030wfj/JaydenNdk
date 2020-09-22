package com.meitu.audiovideo.audio;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.meitu.audiovideo.R;
import com.meitu.audiovideo.Utils;

/**
 * 音频播放
 * Author: wfj
 * Date: 2020/9/18 3:37 PM
 */
public class AudioPlayerActivity extends AppCompatActivity {
    static {
        System.loadLibrary("audiovideo");
    }
    /**
     * PCM 播放实例
     */
    private AudioTracker mAudioTracker;
    private final String PATH = Environment.getExternalStorageDirectory() + "_test.pcm";
    private AudioRecorder mAudioRecorder;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_audio_player);
        mAudioTracker = new AudioTracker(this);
        mAudioRecorder = new AudioRecorder();
        mAudioRecorder.setOnAudioFrameCaptureListener(new AudioRecorder.OnAudioFrameCaptureListener() {
            @Override
            public void onAudioFrameCaptured(byte[] audioData) {
                Utils.writePCM(audioData, PATH);
            }
        });
    }

    /**
     * 使用AudioTrack 播放 PCM
     * @param view
     */
    public void audioTrackPlayPcm(View view) {
        if (mAudioTracker != null) {
            mAudioTracker.createAudioTrack(PATH);
            mAudioTracker.start();
        }
    }

    @Override
    protected void onStop() {
        super.onStop();
        if (mAudioTracker != null) {
            mAudioTracker.stop();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (mAudioTracker != null) {
            mAudioTracker.release();
            mAudioTracker = null;
        }
        nativeStopPcm();
    }

    private native void nativePlayPcm(String pcmPath);
    private native void nativeStopPcm();

    /**
     * 使用OpenSLES播放 PCM
     * @param view
     */
    public void openSLESPlayPcm(View view) {
        Log.d("wfj", "OpenSL ES play PCM!");
        nativePlayPcm(PATH);
    }

    /**
     * 停止使用OpenSLES播放 PCM
     * @param view
     */
    public void stopOpenSLESPlayPcm(View view) {
        Log.d("wfj", "OpenSL ES stop play PCM!");
        nativeStopPcm();
    }


    /**
     * 开始录制音频
     * @param view
     */
    public void startRecordPcm(View view) {
        mAudioRecorder.startCapture();
    }

    /**
     * 停止录制音频
     * @param view
     */
    public void stopRecordPcm(View view) {
        mAudioRecorder.stopCapture();
    }
}

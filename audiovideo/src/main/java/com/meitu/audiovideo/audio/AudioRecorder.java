package com.meitu.audiovideo.audio;

import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

/**
 * 使用系统AudioRecord录制音频
 * Author: wfj
 * Date: 2020/9/22 10:26 PM
 */
public class AudioRecorder {

    //默认录音源
    private static final int DEFAULT_SOURCE = MediaRecorder.AudioSource.MIC;

    //采样率 使用常用的 44100
    private static final int DEFAULT_SAMPLE_RATE = 44100;
    //通道
    private static final int DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
    //采样格式为 16bit
    private static final int DEFAULT_AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT;

    private AudioRecord mAudioRecord;
    private int mMinBufferSize = 0;

    private Thread mCaptureThread;
    private boolean mIsCaptureStarted = false;
    private volatile boolean mIsLoopExit = false;

    private OnAudioFrameCaptureListener mOnAudioFrameCaptureListener;

    public interface OnAudioFrameCaptureListener {
        void onAudioFrameCaptured(byte[] audioData);
    }

    public void setOnAudioFrameCaptureListener(OnAudioFrameCaptureListener mOnAudioFrameCaptureListener) {
        this.mOnAudioFrameCaptureListener = mOnAudioFrameCaptureListener;
    }

    public boolean startCapture() {
        return startCapture(DEFAULT_SOURCE, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_CONFIG, DEFAULT_AUDIO_FORMAT);
    }

    public boolean startCapture(int audioSource, int sampleRateInHz, int channelConfig, int audioFormat) {
        if (mIsCaptureStarted) {
            Log.e("wfj", "Capture already started!");
            return false;
        }

        mMinBufferSize = AudioRecord.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);
        if (mMinBufferSize == AudioRecord.ERROR_BAD_VALUE) {
            Log.e("wfj", "getMinBufferSize Invalid Parameter!!");
            return false;
        }

        Log.d("wfj", "getMinBufferSize = " + mMinBufferSize + " bytes");

        mAudioRecord = new AudioRecord(audioSource, sampleRateInHz, channelConfig, audioFormat, mMinBufferSize);
        if (mAudioRecord.getState() == AudioRecord.STATE_UNINITIALIZED) {
            Log.e("wfj", "AudioRecord initialize fail !");
            return false;
        }

        mAudioRecord.startRecording();

        mIsLoopExit = false;
        mCaptureThread = new Thread(new AudioCaptureRunnable());
        mCaptureThread.start();

        mIsCaptureStarted = true;
        Log.d("wfj", "Start audio capture success !");

        return true;
    }

    /**
     * 停止录制音频
     */
    public void stopCapture() {
        if (!mIsCaptureStarted) {
            return;
        }

        mIsLoopExit = true;
        try {
            mCaptureThread.interrupt();
            mCaptureThread.join(1000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        if (mAudioRecord.getRecordingState() == AudioRecord.RECORDSTATE_RECORDING) {
            mAudioRecord.stop();
        }
        mAudioRecord.release();
        mIsCaptureStarted = false;
        mOnAudioFrameCaptureListener = null;
        Log.d("wfj", "Stop audio capture success !");

    }

    /**
     * 循环录音
     */
    private class AudioCaptureRunnable implements Runnable {
        @Override
        public void run() {
            while (!mIsLoopExit) {
                byte[] buffer = new byte[mMinBufferSize];
                int ret = mAudioRecord.read(buffer, 0, mMinBufferSize);
                if (ret == AudioRecord.ERROR_INVALID_OPERATION) {
                    Log.e("wfj", "Error ERROR_INVALID_OPERATION");
                } else if (ret == AudioRecord.ERROR_BAD_VALUE) {
                    Log.e("wfj", "Error ERROR_BAD_VALUE");
                } else {
                    if (mOnAudioFrameCaptureListener != null) {
                        mOnAudioFrameCaptureListener.onAudioFrameCaptured(buffer);
                    }
                    Log.d("wfj" , "OK, Captured "+ret+" bytes !");
                }
            }
        }
    }
}

package com.meitu.learnffmpeg;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.widget.SeekBar;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.meitu.learnffmpeg.media.FFMediaPlayer;
import com.meitu.learnffmpeg.media.MySurfaceView;

import static com.meitu.learnffmpeg.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_DURATION;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_HEIGHT;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MEDIA_PARAM_VIDEO_WIDTH;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MSG_DECODER_DONE;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MSG_DECODER_INIT_ERROR;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MSG_DECODER_READY;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MSG_DECODING_TIME;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.MSG_REQUEST_RENDER;
import static com.meitu.learnffmpeg.media.FFMediaPlayer.VIDEO_RENDER_ANWINDOW;

/**
 * Author: wfj
 * Date: 2020/8/25 8:50 AM
 */
public class NativeMediaPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, FFMediaPlayer.EventCallback {
    private static final String TAG = "NativePlayerActivity";

    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final int PERMISSION_REQUEST_CODE = 1;

    private MySurfaceView mySurfaceView = null;
    private SeekBar mySeekBar = null;
    private boolean mIsTouch = false;
    private FFMediaPlayer mMediaPlayer = null;
    private String mVideoPath = Environment.getExternalStorageDirectory().getAbsolutePath() + "/jayden/one_piece.mp4";

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_media_player);

        mySurfaceView = findViewById(R.id.surface_view);
        mySeekBar = findViewById(R.id.seek_bar);

        mySurfaceView.getHolder().addCallback(this);
        mySeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
                mIsTouch = true;
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                Log.d(TAG, "onStopTrackingTouch() called with: progress = [" + seekBar.getProgress() + "]");
                if (mMediaPlayer != null) {
                    mMediaPlayer.seekToPosition(seekBar.getProgress());
                    mIsTouch = false;
                }
            }
        });
    }

    private boolean hasPermissionsGranted(String[] permissions) {
        for (String permission: permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, PERMISSION_REQUEST_CODE);
        }
        if (mMediaPlayer != null) {
            mMediaPlayer.play();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (mMediaPlayer != null) {
            mMediaPlayer.pause();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(this, "we need the permission: WRITE_EXTERNAL_STORAGE", Toast.LENGTH_LONG).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        mMediaPlayer = new FFMediaPlayer();
        mMediaPlayer.addEventCallback(this);
        mMediaPlayer.init(mVideoPath, VIDEO_RENDER_ANWINDOW, holder.getSurface());
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        mMediaPlayer.play();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        mMediaPlayer.unInit();
    }

    @Override
    public void onPlayerEvent(final int msgType, final float msgValue) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                switch (msgType) {
                    case MSG_DECODER_DONE:
                    case MSG_REQUEST_RENDER:
                    case MSG_DECODER_INIT_ERROR: {
                        break;
                    }
                    case MSG_DECODER_READY: {
                        onDecoderReady();
                        break;
                    }
                    case MSG_DECODING_TIME: {
                        if (!mIsTouch) {
                            mySeekBar.setProgress((int) msgValue);
                        }
                        break;
                    }
                }
            }
        });
    }

    private void onDecoderReady() {
        int videoHeight = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_HEIGHT);
        int videoWidth = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_WIDTH);
        if (videoHeight * videoWidth != 0) {
            mySurfaceView.setAspectRatio(videoWidth, videoHeight);
        }

        int duration = (int) mMediaPlayer.getMediaParams(MEDIA_PARAM_VIDEO_DURATION);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mySeekBar.setMin(0);
        }
        mySeekBar.setMax(duration);
    }
}

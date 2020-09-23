package com.meitu.audiovideo.video;

import android.os.Bundle;
import android.os.Environment;
import android.view.View;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.meitu.audiovideo.R;

/**
 * Author: wfj
 * Date: 2020/9/23 7:48 AM
 */
public class VideoPlayerActivity extends AppCompatActivity {
    private final String PATH = Environment.getExternalStorageDirectory() + "/yuvtest.yuv";

    private YUVPlayer mYUVPlayer;
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_video_player);
        mYUVPlayer = findViewById(R.id.surface);

    }

    public void videoNativePlay(View view) {
        mYUVPlayer.start(PATH, mYUVPlayer.getHolder().getSurface());
    }

    public void videoNativeStop(View view) {
        mYUVPlayer.stop();
    }
}

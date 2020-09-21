package com.meitu.audiovideo;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

import com.meitu.audiovideo.audio.AudioPlayerActivity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void startAudio(View view) {
        startActivity(new Intent(this, AudioPlayerActivity.class));
    }

    public void startVideo(View view) {
    }
}

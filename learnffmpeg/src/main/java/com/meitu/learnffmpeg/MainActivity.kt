package com.meitu.learnffmpeg

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import com.meitu.learnffmpeg.media.FFMediaPlayer

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val textView = findViewById<TextView>(R.id.textView)
        textView.text = "FFmpeg Version Info:\n" + FFMediaPlayer.getFFmpegVersion()

        val nativePlayer = findViewById<Button>(R.id.nativePlayer)
        nativePlayer.setOnClickListener {
            startActivity(Intent(this, NativeMediaPlayerActivity::class.java))
        }
    }
}

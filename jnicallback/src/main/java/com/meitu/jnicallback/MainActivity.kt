package com.meitu.jnicallback

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.widget.TextView
import kotlin.math.min

class MainActivity : AppCompatActivity() {

    private var jniText: TextView? = null
    private var tickText: TextView? = null
    var hour = 0
    var minute = 0
    var second = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        jniText = findViewById(R.id.hellojniMsg)
        tickText = findViewById(R.id.tickView)
        jniText?.text = stringFromJNI()
    }

    override fun onResume() {
        super.onResume()
        hour = 0
        minute = 0
        second = 0
        startTicks()
    }

    override fun onPause() {
        super.onPause()
        stopTicks()
    }

    /**
     * 此方法在Jni中调用
     */
    fun updateTimer() {
        Log.d("wfj", "thread:${Thread.currentThread()}")
        second++
        if (second >= 60) {
            minute++
            second -= 60
            if (minute >= 60) {
                hour++
                minute -= 60;
            }
        }
        runOnUiThread {
            val ticks = "$hour:$minute:$second"
            tickText?.text = ticks
        }
    }

    external fun startTicks()
    external fun stopTicks()
    external fun stringFromJNI(): String

    companion object {
        init {
            System.loadLibrary("hello-jnicallback")
        }
    }
}

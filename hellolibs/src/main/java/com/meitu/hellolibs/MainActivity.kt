package com.meitu.hellolibs

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.TextView

class MainActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("hello-libs")
        }
    }
    external fun stringFromJNI(): String

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        val textView = findViewById<TextView>(R.id.textview)
        textView.text = stringFromJNI()

        val nativeAndJava = NativeAndJava();
        nativeAndJava.javaCallNative("java send msg to native!!")


        //创建native thread
        val thread = NativeThread().apply {
            createThread()//线程创建
        }
        //线程同步
        val threadMutex = findViewById<Button>(R.id.threadMutex)
        threadMutex.setOnClickListener {
            thread.mutexThread()
        }
    }

    private var jayden: Int = 1
}

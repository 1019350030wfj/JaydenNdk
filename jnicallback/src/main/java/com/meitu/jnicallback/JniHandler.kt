package com.meitu.jnicallback

import android.os.Build
import android.util.Log

/**
 * native call Java
 * 1. private function
 * 2. public static
 * 3. public non-static function
 */
class JniHandler {

    private fun updateStatus(msg: String) {
        if (msg.toLowerCase().contains("error")) {
            Log.e("wfj", "Native error:$msg")
        } else {
            Log.i("wfj", "Native msg:$msg")
        }
    }

    companion object {
        fun getBuildVersion(): String {
            return Build.VERSION.RELEASE
        }
    }

    fun getRuntimeMemory(): Long {
        return Runtime.getRuntime().freeMemory()
    }
}
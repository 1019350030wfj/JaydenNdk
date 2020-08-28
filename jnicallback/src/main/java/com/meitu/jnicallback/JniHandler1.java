package com.meitu.jnicallback;

import android.os.Build;
import android.util.Log;

public class JniHandler1 {

    private void updateStatus(String msg) {
        if (msg.toLowerCase().contains("error")) {
            Log.e("wfj", "Native error:$msg");
        } else {
            Log.i("wfj", "Native msg:$msg");
        }
    }

    public static String getBuildVersion() {
        return Build.VERSION.RELEASE;
    }

    public long getRuntimeMemorySize() {
        return Runtime.getRuntime().freeMemory();
    }
}

package com.meitu.camera;

import android.app.Application;

/**
 * Author: wfj
 * Date: 2020/9/28 1:41 PM
 */
public class CameraApplication extends Application {
    private static Application mBaseApplication = null;
    @Override
    public void onCreate() {
        super.onCreate();
        mBaseApplication = this;
    }

    public static Application getBaseApplication() {
        return getApplication();
    }
    public static Application getApplication() {
        return mBaseApplication;
    }

}

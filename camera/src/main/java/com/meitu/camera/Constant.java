package com.meitu.camera;

import android.util.Log;

import java.io.File;

/**
 * Author: wfj
 * Date: 2020/9/27 9:51 PM
 */
public class Constant {


    public static final String VIDEO_IMG_SUFFIX = ".mp4";


    public static final int MEDIA_QUALITY_HIGH = 30 * 100000;


    public static String getCacheVideoDir() {
        return CameraApplication.getBaseApplication().getExternalFilesDir(null).getAbsolutePath() + File.separator + "video" + File.separator;
    }

}

package com.meitu.camera.view;

public interface CameraOprCallback {
    void capturePic();

    void recordVideo();

    void stopRecord(boolean save);

}

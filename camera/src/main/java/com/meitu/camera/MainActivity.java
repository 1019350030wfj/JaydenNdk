package com.meitu.camera;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.FragmentTransaction;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.view.View;
import android.widget.Toast;

import com.meitu.camera.view.Camera2ShutterBtn;
import com.meitu.camera.view.CameraOprCallback;

public class MainActivity extends AppCompatActivity implements CameraOprCallback {

    private Camera2Fragment camera2Fragment;

    private static final String TAG = "MainActivity";
    private static final String[] REQUEST_PERMISSIONS = {
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.CAMERA,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
    };
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Camera2ShutterBtn captureIv = findViewById(R.id.capture_iv);
        captureIv.setCameraOprCallback(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
            gotoCamera();
        } else {
            ActivityCompat.requestPermissions(this, REQUEST_PERMISSIONS, CAMERA_PERMISSION_REQUEST_CODE);
        }
    }

    private void gotoCamera() {
        camera2Fragment = new Camera2Fragment();
        FragmentTransaction fragmentTransaction = getSupportFragmentManager().beginTransaction();
        fragmentTransaction.add(R.id.camera_container, camera2Fragment, Camera2Fragment.TAG);
        fragmentTransaction.commitNowAllowingStateLoss();
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (hasPermissionsGranted(REQUEST_PERMISSIONS)) {
               gotoCamera();
            } else {
                Toast.makeText(this, "We need the camera permission.", Toast.LENGTH_SHORT).show();
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        }
    }

    protected boolean hasPermissionsGranted(String[] permissions) {
        for (String permission : permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                    != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    /**
     * 切换前后置
     * @param view
     */
    public void switchCameraFace(View view) {
        if (camera2Fragment != null) {
            String cameraId = camera2Fragment.getCameraId();
            String[] cameraIds = camera2Fragment.getSupportCameraIds();
            if (cameraIds != null) {
                for (int i = 0; i < cameraIds.length; i++) {
                    if (!cameraIds[i].equals(cameraId)) {
                        camera2Fragment.updateCameraId(cameraIds[i]);
                        break;
                    }
                }
            }
        }
    }

    @Override
    public void capturePic() {

    }

    @Override
    public void recordVideo() {
        if (camera2Fragment != null) {
            camera2Fragment.startRecord();
        }
    }

    @Override
    public void stopRecord(boolean save) {
        if (camera2Fragment != null) {
            camera2Fragment.stopRecord(true);
        }
    }
}

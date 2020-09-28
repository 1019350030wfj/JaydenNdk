package com.meitu.camera;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.AndroidRuntimeException;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.UUID;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import static com.meitu.camera.Constant.MEDIA_QUALITY_HIGH;

/**
 * Camera2 相机界面
 * Author: wfj
 * Date: 2020/9/26 10:52 AM
 */
public class Camera2Fragment extends Fragment {

    public static final String TAG = "Camera2Fragment";
    private static final int DEFAULT_CAMERA_ID = 0;

    private AutoFitSurfaceView viewFinder;
    private WindowManager windowManager;
    private CameraManager mCameraManager;
    private CameraCharacteristics characteristics;
    private String mCameraId;
    private String[] mSupportCameraIds;

    private ImageReader nv21Reader;

    private CameraCaptureSession mCameraCaptureSession;
    private CaptureRequest mPreviewRequest;
    private CameraDevice mCameraDevice;
    private Semaphore mCameraLock = new Semaphore(1);
    private Size mDefaultPreviewSize = new Size(1920, 1080);
    private Size mDefaultCaptureSize = new Size(1920, 1080);
    private Size mDefaultRecordSize = new Size(1920, 1080);
    private Integer mSensorOrientation;
    private int rotation;
    private static final int SENSOR_ORIENTATION_DEFAULT_DEGREES = 90;
    private static final int SENSOR_ORIENTATION_INVERSE_DEGREES = 270;

    private static final SparseIntArray DEFAULT_ORIENTATIONS = new SparseIntArray();
    private static final SparseIntArray INVERSE_ORIENTATIONS = new SparseIntArray();

    static {
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_0, 90);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_90, 0);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_180, 270);
        DEFAULT_ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }

    static {
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_0, 270);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_90, 180);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_180, 90);
        INVERSE_ORIENTATIONS.append(Surface.ROTATION_270, 0);
    }

    private Size mPreviewSize, mPictureSize, mRecordSize;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;

    private ImageReader.OnImageAvailableListener mOnPreviewImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image = reader.acquireLatestImage();
            if (image != null) {
//                if (mCamera2FrameCallback != null) {
//                    mCamera2FrameCallback.onPreviewFrame(CameraUtil.YUV_420_888_data(image), image.getWidth(), image.getHeight());
//                }
                image.close();
            }
        }
    };

    private ImageReader.OnImageAvailableListener mOnCaptureImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            Image image = reader.acquireLatestImage();
            if (image != null) {
//                if (mCamera2FrameCallback != null) {
//                    mCamera2FrameCallback.onCaptureFrame(CameraUtil.YUV_420_888_data(image), image.getWidth(), image.getHeight());
//                }
                image.close();
            }
        }
    };

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initCamera2Wrapper();
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_camera2, container, false);
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        viewFinder = view.findViewById(R.id.view_finder);

        viewFinder.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
                Size previewSize = CameraSizesKt.getPreviewOutputSize(viewFinder.getDisplay(), characteristics, SurfaceHolder.class, null);
                Log.d(TAG, "View finder size: " + viewFinder.getWidth() + " x " + viewFinder.getHeight());
                Log.d(TAG, "Selected preview size: " + previewSize + " record size:" + mRecordSize);
                viewFinder.setAspectRatio(previewSize.getWidth(), previewSize.getHeight());
                viewFinder.post(new Runnable() {
                    @Override
                    public void run() {
                        startCamera();
                    }
                });
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

            }
        });
    }

    public void startCamera() {
        startBackgroundThread();
        Log.d(TAG, "startCamera mPreviewSize:" + mPreviewSize);
//        if (mPreviewImageReader == null && mPreviewSize != null) {
//            Log.d(TAG, "startCamera 1");
//            mPreviewImageReader = ImageReader.newInstance(mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);
//            mPreviewImageReader.setOnImageAvailableListener(mOnPreviewImageAvailableListener, mBackgroundHandler);
//            mPreviewSurface = mPreviewImageReader.getSurface();
//        }

//        if (mCaptureImageReader == null && mPictureSize != null) {
//            mCaptureImageReader = ImageReader.newInstance(mPictureSize.getWidth(), mPictureSize.getHeight(), ImageFormat.YUV_420_888, 2);
//            mCaptureImageReader.setOnImageAvailableListener(mOnCaptureImageAvailableListener, mBackgroundHandler);
//        }
        openCamera();
    }

    private void openCamera() {
        Context context = getActivity();
        if (context == null) {
            throw new AndroidRuntimeException("Camera2Fragment getActivity is Null: ");
        }
        Log.d(TAG, "openCamera() called");
        if (ContextCompat.checkSelfPermission(context, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            return;
        }

        CameraManager manager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        try {
            if (!mCameraLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }
            manager.openCamera(mCameraId, mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Cannot access the camera." + e.toString());
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera opening.", e);
        }
    }

    private void getCameraInfo(String cameraId) {
        try {
            characteristics = mCameraManager.getCameraCharacteristics(cameraId);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }

        StreamConfigurationMap streamConfigs = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        if (streamConfigs != null) {
            mPreviewSize = getTargetSupportSize(Arrays.asList(streamConfigs.getOutputSizes(SurfaceTexture.class)), mDefaultPreviewSize);
            mPictureSize = getTargetSupportSize(Arrays.asList(streamConfigs.getOutputSizes(ImageFormat.YUV_420_888)), mDefaultCaptureSize);
            mRecordSize = getTargetSupportSize(Arrays.asList(streamConfigs.getOutputSizes(MediaRecorder.class)), mDefaultRecordSize);
        }
        mSensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);
        Log.d(TAG, "initCamera2Wrapper() called mSensorOrientation = " + mSensorOrientation);

    }

    private Size getTargetSupportSize(List<Size> supportPreviewSize, Size defaultSize) {
        boolean supportDefaultSize = false;
        Size sameRatioSize = null;
        float defaultRatio = defaultSize.getWidth() * 1.0f / defaultSize.getHeight();
        Size targetSize = supportPreviewSize.get(0);
        for (Size size : supportPreviewSize) {
            Log.d(TAG, "initCamera2Wrapper() called mSupportPreviewSize " + size.getWidth() + "x" + size.getHeight());
            float ratio = size.getWidth() * 1.0f / size.getHeight();
            if (ratio == defaultRatio) {
                sameRatioSize = size;
            }

            if (defaultSize.getWidth() == size.getWidth() && defaultSize.getHeight() == size.getHeight()) {
                supportDefaultSize = true;
                break;
            }
        }
        if (supportDefaultSize) {
            targetSize = defaultSize;
        } else if(sameRatioSize != null) {
            targetSize = sameRatioSize;
        }
        return targetSize;
    }

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {

        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            Log.d(TAG, "onOnpened " + cameraDevice);
            // This method is called when the camera is opened.  We start camera preview here.
            mCameraLock.release();
            mCameraDevice = cameraDevice;
            createCameraPreviewSession();
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            Log.d(TAG, "onDisconnected");

            mCameraLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            Log.d(TAG, "onError error:" + error);

            mCameraLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }
    };

    private void createCameraPreviewSession() {
        Log.e(TAG, "createCaptureSession called");

        try {
            if (null == mCameraDevice) return;
            mCameraDevice.createCaptureSession(Arrays.asList(viewFinder.getHolder().getSurface()),
                    mSessionStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "createCaptureSession " + e.toString());
        }
    }

    private CameraCaptureSession.StateCallback mSessionStateCallback = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(@NonNull CameraCaptureSession session) {
            Log.e(TAG, "mSessionStateCallback onConfigured");

            mCameraCaptureSession = session;
            try {
                mPreviewRequest = createPreviewRequest();
                if (mPreviewRequest != null) {
                    session.setRepeatingRequest(mPreviewRequest, null, mBackgroundHandler);
                } else {
                    Log.e(TAG, "captureRequest is null");
                }
            } catch (CameraAccessException e) {
                Log.e(TAG, "onConfigured " + e.toString());
            }
        }

        @Override
        public void onConfigureFailed(@NonNull CameraCaptureSession session) {
            Log.e(TAG, "onConfigureFailed");
        }
    };

    private CaptureRequest createPreviewRequest() {
        Log.d(TAG, "createPreviewRequest mCameraDevice:" + mCameraDevice);
        if (null == mCameraDevice) return null;
        try {
            Log.d(TAG, "createPreviewRequest1");
            CaptureRequest.Builder builder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            builder.addTarget(viewFinder.getHolder().getSurface());
            return builder.build();
        } catch (CameraAccessException e) {
            Log.e(TAG, e.getMessage());
            return null;
        }
    }

    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("Camera2Background");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    private void stopBackgroundThread() {
        if (mBackgroundThread != null) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    private void initCamera2Wrapper() {
        Context context = getActivity();
        if (context == null) {
            throw new AndroidRuntimeException("Camera2Fragment getActivity is Null: ");
        }
        windowManager = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        mCameraManager = (CameraManager) context.getSystemService(Context.CAMERA_SERVICE);
        rotation = windowManager.getDefaultDisplay().getRotation();

        try {
            mSupportCameraIds = mCameraManager.getCameraIdList();
            if (checkCameraIdSupport(String.valueOf(DEFAULT_CAMERA_ID))) {
            } else {
                throw new AndroidRuntimeException("Don't support the camera id: " + DEFAULT_CAMERA_ID);
            }
            mCameraId = String.valueOf(DEFAULT_CAMERA_ID);
            getCameraInfo(mCameraId);

        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    public String getCameraId() {
        return mCameraId;
    }

    public String[] getSupportCameraIds() {
        return mSupportCameraIds;
    }

    public void updateCameraId(String cameraId) {
        for (String id : mSupportCameraIds) {
            if (id.equals(cameraId) && !mCameraId.equals(cameraId)) {
                mCameraId = cameraId;
                getCameraInfo(mCameraId);
                stopCamera();
                startCamera();
                break;
            }
        }
    }

    public void stopCamera() {
//        if (mPreviewImageReader != null) {
//            mPreviewImageReader.setOnImageAvailableListener(null, null);
//        }

//        if (mCaptureImageReader != null) {
//            mCaptureImageReader.setOnImageAvailableListener(null, null);
//        }
        closeCamera();
        stopBackgroundThread();
    }

    public void closeCamera() {
        Log.d(TAG, "closeCamera() called");
        try {
            mCameraLock.acquire();
            closePreviewSession();
            if (null != mCameraDevice) {
                mCameraDevice.close();
                mCameraDevice = null;
            }
            if (null != nv21Reader) {
                nv21Reader.close();
                nv21Reader = null;
            }

//            if (null != mCaptureImageReader) {
//                mCaptureImageReader.close();
//                mCaptureImageReader = null;
//            }
        } catch (InterruptedException e) {
            throw new RuntimeException("Interrupted while trying to lock camera closing.", e);
        } finally {
            mCameraLock.release();
        }
    }

    @Override
    public void onStop() {
        super.onStop();
        stopCamera();
    }

    private boolean checkCameraIdSupport(String cameraId) {
        boolean isSupported = false;
        for (String id: mSupportCameraIds) {
            if (cameraId.equals(id)) {
                isSupported = true;
            }
        }
        return isSupported;
    }

    private MediaRecorder mediaRecorder;
    private String currentVideoPath;
    private boolean isRecording = false;

    public void clearCurrentVideoPath() {
        currentVideoPath = null;
    }

    /**
     * 开始录制
     */
    public void startRecord() {
        clearCurrentVideoPath();
        if (null == mCameraDevice || null == mPreviewSize || viewFinder == null || getActivity() == null) {
            return;
        }

        try {
            closePreviewSession();
            setupMediaRecorder();
            setUpPreviewReader();

            final CaptureRequest.Builder recordBuilder = mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_RECORD);
            List<Surface> surfaces = new ArrayList<>();

            // Set up Surface for the camera preview
            Surface previewSurface = viewFinder.getHolder().getSurface();
            surfaces.add(previewSurface);
            recordBuilder.addTarget(previewSurface);

            surfaces.add(nv21Reader.getSurface());
            recordBuilder.addTarget(nv21Reader.getSurface());

            // Set up surface for the MediaRecorder
            Surface recorderSurface = mediaRecorder.getSurface();
            recordBuilder.addTarget(recorderSurface);
            surfaces.add(recorderSurface);

            // start a capture session
            // Once the session starts, we can update the UI and start Recording
            mCameraDevice.createCaptureSession(surfaces, new CameraCaptureSession.StateCallback() {
                @Override
                public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
                    mCameraCaptureSession = cameraCaptureSession;
                    updatePreview(recordBuilder);
                    mediaRecorder.start();
                    isRecording = true;
                }

                @Override
                public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {

                }
            }, mBackgroundHandler);

        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     * 停止录制
     * @param save true save to local
     */
    public void stopRecord(boolean save) {
        try {
            isRecording = false;
            if (mediaRecorder != null) {
                mediaRecorder.stop();
                mediaRecorder.reset();
                mediaRecorder.release();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            mediaRecorder = null;
        }

        if (save) {
            // todo wfj
//            cameraResultCallback.getVideoData(getCurrentVideoPath());
        } else {
            File file = new File(currentVideoPath);
            if (file.exists()) {
                file.delete();
            }
        }
        // 停止录制关闭录制session， 重新创建预览session
        try {
            if (mCameraCaptureSession != null) {
                mCameraCaptureSession.stopRepeating();
                mCameraCaptureSession.abortCaptures();
                mCameraCaptureSession.close();
                mCameraCaptureSession = null;
            }
            createCameraPreviewSession();
        } catch (Exception e) {
            e.printStackTrace();
        }
        clearCurrentVideoPath();
    }

    private void updatePreview(CaptureRequest.Builder recordBuilder) {
        if (null == mCameraDevice) {
            return;
        }
        try {
            recordBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
            mCameraCaptureSession.setRepeatingRequest(recordBuilder.build(), null, mBackgroundHandler);

        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void closePreviewSession() {
        if (mCameraCaptureSession != null) {
            mCameraCaptureSession.close();
            mCameraCaptureSession = null;
        }
    }

    private void setupMediaRecorder() {
        try {
            if (mediaRecorder == null) {
                mediaRecorder = new MediaRecorder();
            }
            mediaRecorder.setAudioSource(MediaRecorder.AudioSource.MIC);
            mediaRecorder.setVideoSource(MediaRecorder.VideoSource.SURFACE);
            mediaRecorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
            String currentVideoName = String.valueOf(UUID.randomUUID());
            String videoFileName = currentVideoName + Constant.VIDEO_IMG_SUFFIX;

            File file = new File(Constant.getCacheVideoDir());
            if (!file.exists()) {
                file.mkdirs();
            }

            currentVideoPath = file.getAbsolutePath() + File.separator + videoFileName;
            File targetFile = new File(currentVideoPath);

            Log.d(TAG, "targetFile:" + targetFile);
            mediaRecorder.setOutputFile(targetFile.getAbsolutePath());
            mediaRecorder.setVideoEncodingBitRate(MEDIA_QUALITY_HIGH);
            mediaRecorder.setVideoFrameRate(30);

            Log.d(TAG, "record width:" + mRecordSize.getWidth() + " " + mRecordSize.getHeight());
            mediaRecorder.setVideoSize(mRecordSize.getWidth(), mRecordSize.getHeight());
            mediaRecorder.setVideoEncoder(MediaRecorder.VideoEncoder.H264);
            mediaRecorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);
            switch (mSensorOrientation) {
                case SENSOR_ORIENTATION_DEFAULT_DEGREES: {
                    mediaRecorder.setOrientationHint(DEFAULT_ORIENTATIONS.get(rotation));
                    break;
                }
                case SENSOR_ORIENTATION_INVERSE_DEGREES: {
                    mediaRecorder.setOrientationHint(INVERSE_ORIENTATIONS.get(rotation));
                    break;
                }
            }
            Log.d(TAG, "mediarecoder prepare");

            mediaRecorder.prepare();
        } catch (Exception e) {
            Log.d(TAG, "setup mediarecoder failed");
            e.printStackTrace();
        }
    }


    private void setUpPreviewReader() {
        nv21Reader = ImageReader.newInstance(mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);
        nv21Reader.setOnImageAvailableListener(new ImageReader.OnImageAvailableListener() {
            @Override
            public void onImageAvailable(ImageReader reader) {
                Image image = reader.acquireLatestImage();
                if (image == null) {
                    return;
                }
                if (image != null) {
                    image.close();
                }
            }
        }, null);


    }
}

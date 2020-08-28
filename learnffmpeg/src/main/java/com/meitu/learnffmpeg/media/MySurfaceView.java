package com.meitu.learnffmpeg.media;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceView;

/**
 * Author: wfj
 * Date: 2020/8/25 8:52 AM
 */
public class MySurfaceView extends SurfaceView {
    private static final String TAG = "MySurfaceView";
    private int mRatioWidth = 0;
    private int mRatioHeight = 0;

    public MySurfaceView(Context context) {
        this(context, null);
    }

    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);
        int width = MeasureSpec.getSize(widthMeasureSpec);
        int height = MeasureSpec.getSize(heightMeasureSpec);

        if (mRatioHeight == 0 || mRatioWidth == 0) {
            setMeasuredDimension(width, height);
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
            }
        }
    }

    public void setAspectRatio(int width, int height) {
        if (width < 0 || height < 0) {
            throw new IllegalArgumentException("Size cannot be negative");
        }

        mRatioHeight = height;
        mRatioWidth = width;
        requestLayout();
    }
}
